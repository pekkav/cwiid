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

#include <iostream>

#include "common.h"
#include "mainwindow.h"
#include "wiimotehandler.h"

MainWindow::MainWindow() : mVBox(false, 0),
                           mConnLabel("No connection"),
                           mConnectBtn("Connect"),
                           mHandler(NULL),
                           mConnStatus(ENotConnected)
{
    // App menu content
    mConnectBtn.show();
    mAppMenu.append(mConnectBtn);
    // Connect button clicked signal handler
    mConnectBtn.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::OnConnectButtonClicked));
    // Add to program as common app menu
    Hildon::Program::get_instance()->set_common_app_menu(mAppMenu);
    
    // Status box
    mConnLabel.show();
    mStatusBox.pack_start(mConnLabel);
    mStatusLabel.show();
    mStatusBox.pack_start(mStatusLabel);
    mExtLabel.show();
    mStatusBox.pack_start(mExtLabel);
    mVBox.pack_start(mStatusBox);

    // Button and motion sensor frames
    mButtonFrame.show();
    mFrameBox.pack_start(mButtonFrame);
    mMotionSensorFrame.show();
    mFrameBox.pack_start(mMotionSensorFrame);
    mVBox.pack_start(mFrameBox);

    // Add container to the window
    add(mVBox);

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

void MainWindow::OnConnectButtonClicked()
{
    if (mConnStatus == ENotConnected) {
        mHandler->Connect();
    } else {
        mHandler->Disconnect();
    }
}

void MainWindow::ShowConnectDialog()
{
    Hildon::Note note(Hildon::NOTE_TYPE_CONFIRMATION_BUTTON,
                      "Put Wiimote in discoverable mode (press 1+2) and press OK");
    note.add_button(Gtk::Stock::OK, Gtk::RESPONSE_YES);
    note.run();
}
