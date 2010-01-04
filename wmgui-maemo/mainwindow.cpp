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
                           mStatusLabel("No connection")
{
    mStatusLabel.show();
    mVBox.pack_start(mStatusLabel);
    // Buttons
    mButtonFrame.show();
    mVBox.pack_start(mButtonFrame);

    // Add widgets to the window
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
            mStatusLabel.set_text("No connection");
            break;
        }
        case EConnecting: {
            mStatusLabel.set_text("Connecting...");
            break;
        }
        case EConnected: {
            mStatusLabel.set_text("Connected");
            break;
        }
        case EConnectionError: {
            mStatusLabel.set_text("Connection error");
            break;
        }
        default: {
            break;
        }
    }
}
