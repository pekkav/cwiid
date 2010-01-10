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

#ifndef __WIIMOTEOBSERVER_H_
#define __WIIMOTEOBSERVER_H_

class IWiimoteObserver
{
public:

    enum ConnStatus {
        ENotConnected = 0,
        EConnecting,
        EConnected,
        EConnectionError
    };

    virtual void ConnectionStatus(ConnStatus /*aStatus*/) {};

    enum WiimoteButton {
        EUp = 0,
        ELeft,
        ERight,
        EDown,
        EA,
        EB,
        EPlus,
        EMinus,
        EHome,
        E1,
        E2
    };

    virtual void ButtonDown(WiimoteButton /*aButton*/) {};
    virtual void ButtonUp(WiimoteButton /*aButton*/) {};

    virtual void BatteryLevel(unsigned int /*aBatteryLevel*/) {};

    enum WiimoteExtension {
        ENone = 0,
        ENunchuck,
        EClassic,
        EBalance,
        EMotionPlus,
        EUnknown
    };

    virtual void CurrentWiimoteExtension(WiimoteExtension /*aExtension*/) {};

    virtual void MotionData(unsigned int /*aXAcc*/,
                            unsigned int /*aYAcc*/,
                            unsigned int /*aZAcc*/,
                            double /*aAcc*/,
                            double /*aRoll*/,
                            double /*aPitch*/) {};
};

#endif // __WIIMOTEOBSERVER_H_
