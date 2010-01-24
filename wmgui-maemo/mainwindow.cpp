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

#include <iostream>

#include "common.h"
#include "mainwindow.h"
#include "wiimotehandler.h"

MainWindow::MainWindow() : mHBox(true, 0),
                           mHandler(NULL)
{
    // App menu content
    mConnectBtn.show();
    mAppMenu.append(mConnectBtn);
    // Connect button clicked signal handler
    mConnectBtn.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::OnConnectButtonClicked));
    // Add to program as common app menu
    Hildon::Program::get_instance()->set_common_app_menu(mAppMenu);

    mButtonFrame1.show();
    mHBox.pack_start(mButtonFrame1);

    mButtonFrame2.show();
    mHBox.pack_start(mButtonFrame2);

    mButtonFrame3.show();
    mHBox.pack_start(mButtonFrame3);

    mButtonFrame4.show();
    mHBox.pack_start(mButtonFrame4);

    // Add container to the window
    add(mHBox);

    // Make everything visible
    show_all();

    mHandler = WiimoteHandler::GetInstance();
    mHandler->AddObserver(this);
}

MainWindow::~MainWindow()
{
    mHandler->RemoveObserver(this);
    WiimoteHandler::Release();
}

void MainWindow::WiimoteConnected(Wiimote *aConnectedWiimote)
{
    ULOG_DEBUG_F();

    if (!mButtonFrame1.HasWiimote()) {
        mButtonFrame1.SetWiimote(aConnectedWiimote);
        return;
    }
}
/*
void MainWindow::ConnectionStatus(ConnStatus aStatus)
{
    ULOG_DEBUG_F("%d", aStatus);

    switch (aStatus) {
        case ENotConnected: {
            mConnLabel.set_text("No connection");
            mStatusLabel.set_text(Glib::ustring::ustring());
            mExtLabel.set_text(Glib::ustring::ustring());
            mConnectBtn.set_label("Connect");
            break;
        }
        case EConnecting: {
            ShowConnectDialog();
            break;
        }
        case EConnected: {
            mConnLabel.set_text("Connected");
            mConnectBtn.set_label("Disconnect");
            break;
        }
        case EConnectionError: {
            mConnLabel.set_text("Connection error");
            break;
        }
        default: {
            break;
        }
    }

    mConnStatus = aStatus;
}

#define BATTERY_FORMAT_STRING "Battery: %1%%"

void MainWindow::BatteryLevel(unsigned int aBatteryLevel)
{
    Glib::ustring batt = Glib::ustring::compose(BATTERY_FORMAT_STRING, aBatteryLevel);
    mStatusLabel.set_text(batt);
}

void MainWindow::CurrentWiimoteExtension(WiimoteExtension aExtension)
{
    Glib::ustring extString;

    switch (aExtension) {
        case ENone: {
            extString.assign("None");
            break;
        }
        case ENunchuck: {
            extString.assign("Nunchuck");
            break;
        }
        case EClassic: {
            extString.assign("Classic");
            break;
        }
        case EBalance: {
            extString.assign("Balance");
            break;
        }
        case EMotionPlus: {
            extString.assign("MotionPlus");
            break;
        }
        case EUnknown: {
            extString.assign("Unknown");
            break;
        }
        default: {
            ULOG_DEBUG_F("Unhandled extension type");
            break;
        }
    }

    mExtLabel.set_text(extString);
}
*/
void MainWindow::OnConnectButtonClicked()
{
    /*
    if (mConnStatus == ENotConnected) {
        mHandler->Connect();
    } else {
        mHandler->Disconnect();
    }
    */
}

void MainWindow::ShowConnectDialog()
{
    Hildon::Note note(Hildon::NOTE_TYPE_CONFIRMATION_BUTTON,
                      "Put Wiimote in discoverable mode (press 1+2) and press OK");
    note.add_button(Gtk::Stock::OK, Gtk::RESPONSE_YES);
    note.run();
}
