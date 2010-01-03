/**
 *  Copyright (C) 2009  Pekka Vanhoja <pekka.vanhoja@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define APP_NAME        "Wiimote GUI for maemo"
#define APP_VERSION     PACKAGE_VERSION
#define APP_COPYRIGHT   "Copyright (C) 2009 Pekka Vanhoja " \
                        "<pekka.vanhoja@gmail.com>"
#define APP_COMMENTS    "Wiimote GUI"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <hildonmm.h>
#include <libosso.h>
#include <bluetooth/bluetooth.h>

#include <cwiid.h>

#include "mainwindow.h"

// Globals
bdaddr_t bdaddr;

// GetOpt
#define OPTSTRING       "h"
extern char *optarg;
extern int optind, opterr, optopt;

// Usage string
#define USAGE "Usage: " << argv[0] << " [-h] [BDADDR]" << std::endl

int
main(int argc,
     char *argv[])
{
    // Init Gtk
    Gtk::Main main_loop(&argc, &argv);
    
    // Init Hildon
    Hildon::init();

    Glib::set_application_name(APP_NAME);
    
    // Init osso context
    osso_context_t* osso_context = osso_initialize("wmgui-maemo",
                                                   APP_VERSION,
                                                   TRUE,
                                                   NULL);

    if (!osso_context) {
        std::cerr << "osso_initialize() failed." << std::endl;
        exit(1);
    }

    if (!g_thread_supported()) {
        g_thread_init(NULL);
    }

    gdk_threads_init();
    gdk_threads_enter();

    // Parse Options
    int c = 0;
    while ((c = getopt(argc, argv, OPTSTRING)) != -1) {
        switch (c) {
            case 'h': {
                std::cout << USAGE;
                return 0;
                break;
            }
            case '?': {
                exit(1);
                break;
            }
            default: {
                std::cout << "Unknown command-line option: -" << c << std::endl;
                break;
            }
        }
    }

    // BDADDR
    char *str_addr = NULL;
    if (optind < argc) {
        if (str2ba(argv[optind], &bdaddr)) {
            std::cout << "Invalid bdaddr" << std::endl;
            bdaddr = *BDADDR_ANY;
        }
        optind++;
        if (optind < argc) {
            std::cerr << "Invalid command-line" << std::endl;
            std::cerr << USAGE;
            exit(1);
        }
    } else if ((str_addr = getenv (WIIMOTE_BDADDR)) != NULL) {
        if (str2ba (str_addr, &bdaddr)) {
            std::cout << "Invalid address in " << WIIMOTE_BDADDR << std::endl;
            bdaddr = *BDADDR_ANY;
        }
    } else {
        bdaddr = *BDADDR_ANY;
    }

    MainWindow main_window;
    Hildon::Program::get_instance()->add_window(main_window);

    main_loop.run(main_window);

    osso_deinitialize(osso_context);
    gdk_threads_leave();

    return 0;
}
