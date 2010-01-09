
#define DBUS_API_SUBJECT_TO_CHANGE

#include <glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include "cwiid_internal.h"
#include "marshal.h"

#define OSSOLOG_COMPILE
#define OSSOLOG_STDOUT
#include <osso-log.h>

// BlueZ DBus API strings
#define BLUEZ_NAME "org.bluez"
#define BLUEZ_MANAGER_PATH "/"
#define BLUEZ_MANAGER_INTERFACE "org.bluez.Manager"
#define BLUEZ_ADAPTER_INTERFACE "org.bluez.Adapter"

typedef struct {
    DBusGConnection *sys_dbus;
    DBusGProxy *manager_proxy;
    DBusGProxy *adapter_proxy;
    gchar *default_adapter;
    GHashTable *adapter_properties;
} BackgroundSearchData;

static BackgroundSearchData *search_data = NULL;

static void clean_data(BackgroundSearchData *data)
{
    if (data->adapter_properties) {
        g_hash_table_destroy(data->adapter_properties);
    }
    g_free(data->default_adapter);
    if (data->adapter_proxy) {
        g_object_unref(G_OBJECT(data->adapter_proxy));
    }
    if (data->manager_proxy) {
        g_object_unref(G_OBJECT(data->manager_proxy));
    }
}

static void device_found(DBusGProxy *adapter,
                         const gchar *address,
                         GHashTable *properties,
                         gpointer user_data)
{
    ULOG_DEBUG_F("A Bluetooth device is found at: %s", address);
}

static gboolean start_wiimote_query(gpointer user_data)
{
    GError *error = NULL;
    BackgroundSearchData *data = (BackgroundSearchData *)user_data;

    ULOG_DEBUG_F("Starting device discovery");

    /* Start listening DeviceFound signals */
    dbus_g_object_register_marshaller(marshal_VOID__STRING_POINTER,
                                      G_TYPE_NONE,
                                      G_TYPE_STRING,
                                      G_TYPE_POINTER,
                                      G_TYPE_INVALID);

    dbus_g_proxy_add_signal(data->adapter_proxy,
                            "DeviceFound",
                            G_TYPE_STRING,
                            dbus_g_type_get_map("GHashTable",
                                                G_TYPE_STRING, G_TYPE_VALUE),
                            G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(data->adapter_proxy,
                                "DeviceFound",
                                G_CALLBACK(device_found),
                                data,
                                NULL);

    /* Start device discovery */
    if (!dbus_g_proxy_call(data->adapter_proxy,
                           "StartDiscovery",
                           &error,
                           G_TYPE_INVALID,
                           G_TYPE_INVALID)) {
        g_clear_error(&error);
        clean_data(data);
        g_free(data);
        search_data = NULL;
        /* TODO: Error handling */
        ULOG_ERR_F("Error in starting discovery");
        return FALSE;
    }

    return FALSE;
}

static gboolean check_adapter_powered(gpointer user_data)
{
    GError *error = NULL;
    BackgroundSearchData *data = (BackgroundSearchData *)user_data;

    GValue *value;
    gboolean powered;
    value = g_hash_table_lookup(data->adapter_properties, "Powered");
    powered = value ? g_value_get_boolean(value) : FALSE;
    
    ULOG_DEBUG_F("default adapter: %s, adapter powered: %d",
                 data->default_adapter, powered);

    /* Turn adapter on if it was turned off */
    if (value && !powered) {
        GValue new_value = { 0, };
        g_value_init(&new_value, G_TYPE_BOOLEAN);
        g_value_set_boolean(&new_value, TRUE);
        if (!dbus_g_proxy_call(data->adapter_proxy,
                               "SetProperty",
                               &error,
                               G_TYPE_STRING, "Powered",
                               G_TYPE_VALUE, &new_value,
                               G_TYPE_INVALID,
                               G_TYPE_INVALID)) {
            g_clear_error(&error);
            clean_data(data);
            g_free(data);
            search_data = NULL;
            /* TODO: Error handling */
            ULOG_ERR_F("Error in setting adapter on\n");
            return FALSE;
        }
    }

    if (!g_idle_add(start_wiimote_query, data)) {
        /* TODO: Error handling */
    }

    return FALSE;
}

static gboolean get_adapter_properties(gpointer user_data)
{
    GError *error = NULL;
    BackgroundSearchData *data = (BackgroundSearchData *)user_data;

    /* Get adapter properties to chech if the adapter is turned on */
    if (!dbus_g_proxy_call(data->adapter_proxy,
                           "GetProperties",
                           &error,
                           G_TYPE_INVALID,
                           dbus_g_type_get_map("GHashTable",
                                               G_TYPE_STRING, G_TYPE_VALUE),
                           &data->adapter_properties,
                           G_TYPE_INVALID)) {
        g_clear_error(&error);
        clean_data(data);
        g_free(data);
        search_data = NULL;
        /* TODO: Error handling */
        ULOG_ERR_F("Error getting default adapter properties");
        return FALSE;
    }

    if (!g_idle_add(check_adapter_powered, data)) {
        /* TODO: Error handling */
    }

    return FALSE;
}

static void adapter_property_changed(DBusGProxy *adapter,
                                     const gchar *property,
                                     GValue *value,
                                     gpointer user_data)
{
    ULOG_DEBUG_F("Changed property: '%s'", property);
}

static gboolean get_default_adapter(gpointer user_data)
{
    ULOG_DEBUG_F();

    GError *error = NULL;
    BackgroundSearchData *data = (BackgroundSearchData *)user_data;

    data->manager_proxy = dbus_g_proxy_new_for_name(data->sys_dbus,
                                                    BLUEZ_NAME,
                                                    BLUEZ_MANAGER_PATH,
                                                    BLUEZ_MANAGER_INTERFACE);

    /* Get default adapter object path from manager */
    if (!dbus_g_proxy_call(data->manager_proxy,
                           "DefaultAdapter",
                           &error,
                           G_TYPE_INVALID,
                           DBUS_TYPE_G_OBJECT_PATH, &data->default_adapter,
                           G_TYPE_INVALID)) {
        g_clear_error(&error);
        clean_data(data);
        g_free(data);
        search_data = NULL;
        /* TODO: Error handling */
        ULOG_ERR_F("Error getting default adapter path");
        return FALSE;
    }

    /* Get default adapter */
    data->adapter_proxy = /*dbus_g_proxy_new_for_name(data->sys_dbus,
                                                    BLUEZ_NAME,
                                                    data->default_adapter,
                                                    BLUEZ_ADAPTER_INTERFACE);*/
        dbus_g_proxy_new_from_proxy(data->manager_proxy,
                                    BLUEZ_ADAPTER_INTERFACE,
                                    data->default_adapter);


    /* Start listening PropertyChanged signals from adapter */
    dbus_g_object_register_marshaller(marshal_VOID__STRING_BOXED,
                                      G_TYPE_NONE,
                                      G_TYPE_STRING,
                                      G_TYPE_VALUE,
                                      G_TYPE_INVALID);

    dbus_g_proxy_add_signal(data->adapter_proxy,
                            "PropertyChanged",
                            G_TYPE_STRING,
                            G_TYPE_VALUE,
                            G_TYPE_INVALID);

    dbus_g_proxy_connect_signal(data->adapter_proxy,
                                "PropertyChanged",
                                G_CALLBACK(adapter_property_changed),
                                data,
                                NULL);

    if (!g_idle_add(get_adapter_properties, data)) {
        /* TODO: Error handling */
    }

    return FALSE;
}

int cwiid_start_background_seach(/*cwiid_wiimote_connected_callback_t *callback*/)
{
    if (search_data)
        return -1;

    GError *error = NULL;
    DBusGConnection *sys_dbus;

    /* Get system dbus connection */
    sys_dbus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
    if (error) {
        g_clear_error(&error);
        return -1;
    }

    BackgroundSearchData *data = g_try_new0(BackgroundSearchData, 1);
    if (!data) {
        return -1;
    }

    data->sys_dbus = sys_dbus;

    if (!g_idle_add(get_default_adapter, data)) {
        clean_data(data);
        g_free(data);
        return -1;
    }

    search_data = data;
    return 1;
}
