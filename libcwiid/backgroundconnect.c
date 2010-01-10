/**
 *  Copyright (C) 2010  Pekka Vanhoja <pekka.vanhoja@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define DBUS_API_SUBJECT_TO_CHANGE

#include <glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <bluetooth/bluetooth.h>
#include "cwiid_internal.h"
#include "marshal.h"

//#define OSSOLOG_COMPILE
//#define OSSOLOG_STDOUT
#include <osso-log.h>

/* BlueZ DBus API strings */
#define BLUEZ_NAME "org.bluez"
#define BLUEZ_MANAGER_PATH "/"
#define BLUEZ_MANAGER_INTERFACE "org.bluez.Manager"
#define BLUEZ_ADAPTER_INTERFACE "org.bluez.Adapter"

/* Background search data struct */
typedef struct {
    cwiid_background_search_callback_t *callback;
    DBusGConnection *sys_dbus;
    DBusGProxy *manager_proxy;
    DBusGProxy *adapter_proxy;
    gchar *default_adapter;
    GHashTable *adapter_properties;
    bdaddr_t *new_wiimote_addr;
    GList *connected_wiimotes;
    gboolean adapter_discovering;
} BackgroundSearchData;

static BackgroundSearchData *search_data = NULL;

/* Function prototypes */
static gboolean get_default_adapter(gpointer user_data);
static gboolean get_adapter_properties(gpointer user_data);
static gboolean check_adapter_powered(gpointer user_data);
static gboolean start_device_discovery(gpointer user_data);
static void setup_device_signals(BackgroundSearchData *data);
static gboolean open_wiimote(gpointer user_data);
static void clean_data(BackgroundSearchData *data);
static void adapter_property_changed(DBusGProxy *adapter,
                                     const gchar *property,
                                     GValue *value,
                                     gpointer user_data);
static void device_found(DBusGProxy *adapter,
                         const gchar *address,
                         GHashTable *properties,
                         gpointer user_data);
static void device_disappeared(DBusGProxy *adapter,
                               const gchar *address,
                               gpointer user_data);
static void device_created(DBusGProxy *adapter,
                           const gchar *object_path,
                                gpointer user_data);
static void device_removed(DBusGProxy *adapter,
                           const gchar *object_path,
                           gpointer user_data);

/**
 * 
 * 
 */
int cwiid_start_background_seach(cwiid_background_search_callback_t *callback)
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

    data->callback = callback;
    data->new_wiimote_addr = bt_malloc(sizeof(bdaddr_t));
    data->sys_dbus = sys_dbus;

    if (!g_idle_add(get_default_adapter, data)) {
        clean_data(data);
        g_free(data);
        return -1;
    }

    search_data = data;
    return 1;
}

/**
 * 
 * 
 */
void cwiid_stop_background_searh()
{
    if (!search_data) {
        return;
    }

    if (search_data->adapter_discovering) {
        /* TODO: Error handling */
        dbus_g_proxy_call(search_data->adapter_proxy,
                          "StopDiscovery",
                          NULL,
                          G_TYPE_INVALID,
                          G_TYPE_INVALID);
    }

    clean_data(search_data);
    g_free(search_data);
    search_data = NULL;
}

/**
 * 
 * 
 */
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
    data->adapter_proxy = dbus_g_proxy_new_from_proxy(data->manager_proxy,
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

/**
 * 
 * 
 */
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

/**
 * 
 * 
 */
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

    if (!g_idle_add(start_device_discovery, data)) {
        /* TODO: Error handling */
    }

    return FALSE;
}

/**
 * 
 * 
 */
static gboolean start_device_discovery(gpointer user_data)
{
    GError *error = NULL;
    BackgroundSearchData *data = (BackgroundSearchData *)user_data;

    ULOG_DEBUG_F("Starting device discovery");

    /* Start listening device signals */
    setup_device_signals(data);

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

/**
 * 
 * 
 */
static void setup_device_signals(BackgroundSearchData *data)
{
    /* DeviceFound */
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

    /* DeviceDisappeared */
    dbus_g_object_register_marshaller(marshal_VOID__STRING,
                                      G_TYPE_NONE,
                                      DBUS_TYPE_G_OBJECT_PATH,
                                      G_TYPE_INVALID);

    dbus_g_proxy_add_signal(data->adapter_proxy,
                            "DeviceDisappeared",
                            DBUS_TYPE_G_OBJECT_PATH,
                            G_TYPE_INVALID);

    dbus_g_proxy_connect_signal(data->adapter_proxy,
                                "DeviceDisappeared",
                                G_CALLBACK(device_disappeared),
                                data,
                                NULL);

    /* DeviceCreated */
    dbus_g_proxy_add_signal(data->adapter_proxy,
                            "DeviceCreated",
                            DBUS_TYPE_G_OBJECT_PATH,
                            G_TYPE_INVALID);

    dbus_g_proxy_connect_signal(data->adapter_proxy,
                                "DeviceCreated",
                                G_CALLBACK(device_created),
                                data,
                                NULL);

    /* DeviceRemoved */
    dbus_g_proxy_add_signal(data->adapter_proxy,
                            "DeviceRemoved",
                            DBUS_TYPE_G_OBJECT_PATH,
                            G_TYPE_INVALID);

    dbus_g_proxy_connect_signal(data->adapter_proxy,
                                "DeviceRemoved",
                                G_CALLBACK(device_removed),
                                data,
                                NULL);

}

/**
 * 
 * 
 */
static gboolean open_wiimote(gpointer user_data)
{
    BackgroundSearchData *data = (BackgroundSearchData *)user_data;

    cwiid_wiimote_t *wiimote = cwiid_open(data->new_wiimote_addr, CWIID_FLAG_MESG_IFC);
    ULOG_DEBUG_F("wiimote: %p", wiimote);

    if (wiimote) {
        data->callback(wiimote, 0, NULL);
    } else {
        data->callback(NULL, -1, "Could not connect to a Wiimote");
    }

    /* Start device discovery again */
    if (!g_idle_add(start_device_discovery, data)) {
        /* TODO: Error handling */
    }

    return FALSE;
}

/**
 * 
 * 
 */
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
    if (data->new_wiimote_addr) {
        bt_free(data->new_wiimote_addr);
    }
    if (data->connected_wiimotes) {
        g_list_foreach(data->connected_wiimotes, g_free, NULL);
        g_list_free(data->connected_wiimotes);
    }
}

/**
 * 
 * 
 */
static void adapter_property_changed(DBusGProxy *adapter,
                                     const gchar *property,
                                     GValue *value,
                                     gpointer user_data)
{
    ULOG_DEBUG_F("Changed property: '%s'", property);

    BackgroundSearchData *data = (BackgroundSearchData *)user_data;

    if (g_strcmp0(property, "Discovering") == 0) {
        gboolean discovering = g_value_get_boolean(value);
        ULOG_DEBUG_F("Discovering: %d", discovering);
        data->adapter_discovering = discovering;
    }

    if (g_strcmp0(property, "Devices") == 0) {
        GStrv devices = g_value_get_boxed(value);
        guint devices_count = g_strv_length(devices);
        ULOG_DEBUG_F("%d devices", devices_count);        
    }
}

/**
 * 
 * 
 */
static void device_found(DBusGProxy *adapter,
                         const gchar *address,
                         GHashTable *properties,
                         gpointer user_data)
{
    ULOG_DEBUG_F("A Bluetooth device is found at: %s", address);

    BackgroundSearchData *data = (BackgroundSearchData *)user_data;

    GValue *name_val = g_hash_table_lookup(properties, "Name");
    gchar *name = name_val ? g_value_get_string(name_val) : NULL;
    ULOG_DEBUG_F("Device name: %s", name);

    if (g_strcmp0(name, WIIMOTE_NAME) == 0 &&
        g_list_length(data->connected_wiimotes) < 4) {
        if (str2ba(address, data->new_wiimote_addr)) {
            ULOG_ERR_F("Invalid bdaddr");
        } else {
            /* Stop device discovery while opening the wiimote */
            /* TODO: Error handling */
            dbus_g_proxy_call(data->adapter_proxy,
                              "StopDiscovery",
                              NULL,
                              G_TYPE_INVALID,
                              G_TYPE_INVALID);
            if (!g_idle_add(open_wiimote, data)) {
                /* TODO: Error handling */
            }
        }
    }
}

/**
 * 
 * 
 */
static void device_disappeared(DBusGProxy *adapter,
                               const gchar *address,
                               gpointer user_data)
{
    ULOG_DEBUG_F("address: %s", address);
}

/**
 * 
 * 
 */
static void device_created(DBusGProxy *adapter,
                           const gchar *object_path,
                           gpointer user_data)
{
    ULOG_DEBUG_F("object_path: %s", object_path);

    BackgroundSearchData *data = (BackgroundSearchData *)user_data;

    data->connected_wiimotes = g_list_prepend(data->connected_wiimotes,
                                              g_strdup(object_path));
}

/**
 * 
 * 
 */
static void device_removed(DBusGProxy *adapter,
                           const gchar *object_path,
                           gpointer user_data)
{
    ULOG_DEBUG_F("object_path: %s", object_path);

    BackgroundSearchData *data = (BackgroundSearchData *)user_data;

    GList *item = NULL;
    if (data->connected_wiimotes &&
        (item = g_list_find(data->connected_wiimotes, object_path)) != NULL) {
        g_free(item->data);
        data->connected_wiimotes = g_list_delete_link(data->connected_wiimotes,
                                                      item);
    }
}
