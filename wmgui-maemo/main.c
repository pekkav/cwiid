/* Copyright (C) 2009 Pekka Vanhoja <pekka.vanhoja@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define APP_NAME        "CWiid wmgui for maemo"
#define APP_VERSION        PACKAGE_VERSION
#define APP_COPYRIGHT    "Copyright (C) 2009 Pekka Vanhoja " \
                        "<pekka.vanhoja@gmail.com>"
#define APP_COMMENTS    "Wiimote GUI"

#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <bluetooth/bluetooth.h>
#include <hildon/hildon.h>
#include <libosso.h>

#include "cwiid.h"

/* Globals */
osso_context_t *osso_context;
bdaddr_t bdaddr;

/* GetOpt */
#define OPTSTRING    "h"
extern gchar *optarg;
extern gint optind, opterr, optopt;

/* Usage string */
#define USAGE "usage:%s [-h] [BDADDR]\n"

/* Function prototypes */
static gint
dbus_callback (const gchar *interface,
               const gchar *method,
               GArray *arguments,
               gpointer data,
               osso_rpc_t *retval);

int
main (int argc,
      char *argv[])
{
    gint c;
    gchar *str_addr;
    osso_return_t ret;
    GtkWidget *window;

    osso_context = osso_initialize ("wmgui", APP_VERSION, TRUE, NULL);
    if (!osso_context) {
        printf ("Unable to initialize osso context.\n");
        exit (1);
    }

    gtk_set_locale ();
    hildon_gtk_init (&argc, &argv);

    if (!g_thread_supported ()) {
        g_thread_init (NULL);
    }

    gdk_threads_init ();
    gdk_threads_enter();

    /* Parse Options */
    while ((c = getopt (argc, argv, OPTSTRING)) != -1) {
        switch (c) {
            case 'h': {
                printf(USAGE, argv[0]);
                return 0;
                break;
            }
            case '?': {
                return -1;
                break;
            }
            default: {
                printf("Unknown command-line option: -%c\n", c);
                break;
            }
        }
    }

    /* BDADDR */
    if (optind < argc) {
        if (str2ba (argv[optind], &bdaddr)) {
            printf ("Invalid bdaddr\n");
            bdaddr = *BDADDR_ANY;
        }
        optind++;
        if (optind < argc) {
            printf ("Invalid command-line\n");
            printf (USAGE, argv[0]);
            return -1;
        }
    } else if ((str_addr = getenv (WIIMOTE_BDADDR)) != NULL) {
        if (str2ba (str_addr, &bdaddr)) {
            printf ("Invalid address in %s\n", WIIMOTE_BDADDR);
            bdaddr = *BDADDR_ANY;
        }
    } else {
        bdaddr = *BDADDR_ANY;
    }

    window = hildon_window_new ();

    ret = osso_rpc_set_default_cb_f (osso_context, dbus_callback, window);
    if (ret != OSSO_OK) {
        printf ("Failed to set default rpc callback\n");
        exit (1);
    }

    gtk_widget_show (window);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    gtk_main ();
    gdk_threads_leave ();

    return 0;
}

static gint
dbus_callback (const gchar *interface,
               const gchar *method,
               GArray *arguments,
               gpointer data,
               osso_rpc_t *retval)
{
    if (!g_strcmp0 (method, "top_application")) {
        gtk_window_present (GTK_WINDOW (data));
    }

    retval->type = DBUS_TYPE_INVALID;
    return OSSO_OK;
}
