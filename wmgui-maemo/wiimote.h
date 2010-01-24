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

#ifndef __WIIMOTE_H_
#define __WIIMOTE_H_

#include <cwiid.h>
#include <list>
#include <map>

#include "wiimoteobserver.h"

using namespace std;

class Wiimote
{
public:

    Wiimote(cwiid_wiimote_t* aWiimote);

    virtual ~Wiimote();

    inline cwiid_wiimote_t* GetCWiidWiimote() const { return mWiimote; };

    void AddObserver(const IWiimoteObserver* aObserver);

    void RemoveObserver(const IWiimoteObserver* aObserver);

    void StartReporting(uint8_t aReportMode);

    void Disconnect();

    static void CWiidCallback(cwiid_wiimote_t *aWiimote,
                              int aMsgCount,
                              union cwiid_mesg aMsgArray[],
                              struct timespec *aTimestamp);

    void StatusCallback(struct cwiid_status_mesg *aMsg);

    void ButtonCallback(struct cwiid_btn_mesg *aMsg);

private:

    void NotifyButtonDown(IWiimoteObserver::WiimoteButton aButton);

    void NotifyButtonUp(IWiimoteObserver::WiimoteButton aButton);

    void NotifyConnectionStatus(IWiimoteObserver::ConnStatus aStatus);

private:

    static map<cwiid_wiimote_t*, Wiimote*> mWiimoteMap;

    cwiid_wiimote_t* mWiimote;

    struct acc_cal mWmCal;

    list<const IWiimoteObserver*> mObservers;
};

#endif // __WIIMOTE_H_
