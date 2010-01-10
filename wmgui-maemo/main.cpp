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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define APP_NAME        "Wiimote GUI for maemo"
#define APP_VERSION     PACKAGE_VERSION
#define APP_COPYRIGHT   "Copyright (C) 2010 Pekka Vanhoja " \
                        "<pekka.vanhoja@gmail.com>"
#define APP_COMMENTS    "Wiimote GUI"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <glibmm.h>
#include <hildonmm.h>
#include <libosso.h>
#include <bluetooth/bluetooth.h>


#include "wiimotehandler.h"
#include "mainwindow.h"

// GetOpt
#define OPTSTRING       "h"
extern char* optarg;
extern int optind, opterr, optopt;

// Usage string
#define USAGE "Usage: " << argv[0] << " [-h] [BDADDR]" << std::endl

int
main(int argc,
     char* argv[])
{
    // Init Gtk
    Gtk::Main mainLoop(&argc, &argv);
    
    // Init Hildon
    Hildon::init();

    Glib::set_application_name(APP_NAME);
    
    // Init osso context
    osso_context_t* ossoContext = osso_initialize("wmgui-maemo",
                                                  APP_VERSION,
                                                  true,
                                                  NULL);

    if (!ossoContext) {
        std::cerr << "Initializing osso context failed" << std::endl;
        exit(1);
    }

    if (!Glib::thread_supported()) {
        Glib::thread_init(NULL);
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

    // Get Wiimote handler
    WiimoteHandler* wiimoteHandler = WiimoteHandler::GetInstance();
    if (!wiimoteHandler) {
        std::cerr << "Didn't get wiimote handler" << std::endl;
        exit(1);
    }

    // BDADDR
    char* strAddr = NULL;
    bdaddr_t* bdAddr = wiimoteHandler->GetBdAddr();
    if (!bdAddr) {
        std::cerr << "Didn't get bdaddr pointer from wiimote handler" << std::endl;
        exit(1);
    }

    if (optind < argc) {
        if (str2ba(argv[optind], bdAddr)) {
            std::cout << "Invalid bdaddr" << std::endl;
            bacpy(bdAddr, BDADDR_ANY);
        }
        optind++;
        if (optind < argc) {
            std::cerr << "Invalid command-line" << std::endl;
            std::cerr << USAGE;
            exit(1);
        }
    } else if ((strAddr = getenv (WIIMOTE_BDADDR)) != NULL) {
        if (str2ba (strAddr, bdAddr)) {
            std::cout << "Invalid address in " << WIIMOTE_BDADDR << std::endl;
            bacpy(bdAddr, BDADDR_ANY);
        }
    } else {
        bacpy(bdAddr, BDADDR_ANY);
    }

    // Create our window
    MainWindow mainWindow;
    Hildon::Program::get_instance()->add_window(mainWindow);

    // Start cwiid background search for Wiimotes
    cwiid_start_background_seach(&WiimoteHandler::CWiidBackgrounSearchCallback);

    // Run Gtk main loop
    mainLoop.run(mainWindow);

    // Clean-up
    cwiid_stop_background_searh();
    WiimoteHandler::Release();
    osso_deinitialize(ossoContext);
    gdk_threads_leave();

    return 0;
}
