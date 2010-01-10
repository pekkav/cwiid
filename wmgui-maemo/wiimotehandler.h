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

#ifndef __WIIMOTEHANDLER_H_
#define __WIIMOTEHANDLER_H_

#include <cwiid.h>
#include <bluetooth/bluetooth.h>
#include <list>

#include "wiimoteobserver.h"

using namespace std;

class WiimoteHandler
{
public:

    static WiimoteHandler* GetInstance();
    static void Release();

    bdaddr_t* GetBdAddr() const;
    void SetBdAddr(const bdaddr_t* aBdAddr);
    
    void Connect();
    void Disconnect();

    void AddObserver(const IWiimoteObserver* aObserver);
    void RemoveObserver(const IWiimoteObserver* aObserver);

    void NotifyConnectionStatus(IWiimoteObserver::ConnStatus aStatus);

    void NotifyButtonDown(IWiimoteObserver::WiimoteButton aButton);
    void NotifyButtonUp(IWiimoteObserver::WiimoteButton aButton);

    void NotifyBattery(unsigned int aBatteryLevel);

    void NotifyExtension(IWiimoteObserver::WiimoteExtension aExtType);

    void NotifyMotionData(unsigned int aXAcc,
                          unsigned int aYAcc,
                          unsigned int aZAcc,
                          double aAcc,
                          double aRoll,
                          double aPitch);

    void GetAccelerometerCalibration(struct acc_cal* aCal);

    static void CWiidBackgrounSearchCallback(cwiid_wiimote_t* aWiimote,
                                             int aError,
                                             const char *aErrorMsg);

    void WiimoteConnected(cwiid_wiimote_t* aWiimote);

private:

    WiimoteHandler();

    virtual ~WiimoteHandler();

private:

    static WiimoteHandler* mInstance;
    static unsigned int mRefCount;

    cwiid_wiimote_t* mWiimote;

    IWiimoteObserver::WiimoteExtension mExtType;

    bdaddr_t* mBdAddr;

    struct acc_cal mWmCal;

    list<const IWiimoteObserver*> mObservers;
};

#endif // __WIIMOTEHANDLER_H_
