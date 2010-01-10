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

#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include <hildonmm.h>

#include "buttonframe.h"
#include "motionsensorframe.h"
#include "wiimoteobserver.h"

class WiimoteHandler;

class MainWindow : public Hildon::StackableWindow,
                   public IWiimoteObserver
{
public:

    MainWindow();

    virtual ~MainWindow();

    void ConnectionStatus(ConnStatus aStatus);

    void BatteryLevel(unsigned int aBatteryLevel);

    void CurrentWiimoteExtension(WiimoteExtension aExtension);

private:

    void OnConnectButtonClicked();

    void ShowConnectDialog();

private:

    // Child widgets
    Gtk::VBox mVBox;

    Gtk::HBox mStatusBox;
    Gtk::Label mConnLabel;
    Gtk::Label mStatusLabel;
    Gtk::Label mExtLabel;

    Gtk::HBox mFrameBox;
    ButtonFrame mButtonFrame;
    MotionSensorFrame mMotionSensorFrame;

    // App menu widgets
    Hildon::AppMenu mAppMenu;
    Gtk::Button mConnectBtn;

    WiimoteHandler *mHandler;

    ConnStatus mConnStatus;
};

#endif // __MAINWINDOW_H_
