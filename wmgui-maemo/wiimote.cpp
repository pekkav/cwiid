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

#include <gdk/gdk.h>

#include "common.h"
#include "wiimote.h"

// Wiimote callback
cwiid_mesg_callback_t cwiid_callback;
map<cwiid_wiimote_t*, Wiimote*> Wiimote::mWiimoteMap;

Wiimote::Wiimote(cwiid_wiimote_t* aWiimote) : mWiimote(aWiimote)
                                   
{
    mWiimoteMap.insert(make_pair(aWiimote, this));
}

Wiimote::~Wiimote()
{
    mWiimoteMap.erase(mWiimote);
}

void Wiimote::AddObserver(const IWiimoteObserver* aObserver)
{
    list<const IWiimoteObserver*>::iterator i;
    for (i = mObservers.begin(); i != mObservers.end(); ++i) {
        if (*i == aObserver) {
            ULOG_DEBUG_F("Observer %p already added", aObserver);
            return;
        }
    }

    mObservers.push_back(aObserver);
}

void Wiimote::RemoveObserver(const IWiimoteObserver* aObserver)
{
    list<const IWiimoteObserver*>::iterator i;
    for (i = mObservers.begin(); i != mObservers.end(); ++i) {
        if (*i == aObserver) {
            mObservers.erase(i);
            return;
        }
    }
}

void Wiimote::StartReporting(uint8_t aReportMode)
{
    ULOG_DEBUG_F();

    if (cwiid_set_mesg_callback(mWiimote, &Wiimote::CWiidCallback)) {
        ULOG_DEBUG_F("Error setting callback");
        if (cwiid_close(mWiimote)) {
            ULOG_DEBUG_F("Error on disconnect");
        }
        mWiimote = NULL;
        NotifyConnectionStatus(IWiimoteObserver::EConnectionError);
        NotifyConnectionStatus(IWiimoteObserver::ENotConnected);
    } else {
        ULOG_DEBUG_F("Connected");
        if (cwiid_get_acc_cal(mWiimote, CWIID_EXT_NONE, &mWmCal)) {
            ULOG_DEBUG_F("Unable to retrieve accelerometer calibration");
        }

        NotifyConnectionStatus(IWiimoteObserver::EConnected);

        /*
        set_gui_state();
        set_report_mode();
        cwiid_enable(wiimote, CWIID_FLAG_MOTIONPLUS);
        */

        //uint8_t rpt_mode;    
        //rpt_mode = CWIID_RPT_STATUS | CWIID_RPT_BTN | CWIID_RPT_ACC;
        if (cwiid_set_rpt_mode(mWiimote, aReportMode)) {
            ULOG_DEBUG_F("Error setting report mode");
        }

        cwiid_request_status(mWiimote);
    }
}

void Wiimote::Disconnect()
{
    if (mWiimote) {
        if (cwiid_close(mWiimote)) {
            ULOG_DEBUG_F("Error on disconnect");
        }
        NotifyConnectionStatus(IWiimoteObserver::ENotConnected);
    }
}

void Wiimote::CWiidCallback(cwiid_wiimote_t *aWiimote,
                            int aMsgCount,
                            union cwiid_mesg aMsgArray[],
                            struct timespec *aTimestamp)
{
    ULOG_DEBUG_F();
    Wiimote *wiimote = mWiimoteMap[aWiimote];

    gdk_threads_enter();

    for (int i = 0; i < aMsgCount; i++) {
        switch (aMsgArray[i].type) {
            case CWIID_MESG_STATUS: {
                wiimote->StatusCallback(&aMsgArray[i].status_mesg);
                break;
            }
            case CWIID_MESG_BTN: {
                wiimote->ButtonCallback(&aMsgArray[i].btn_mesg);
                break;
            }
            case CWIID_MESG_ACC: {
                //cwiid_acc(&aMsgArray[i].acc_mesg);
                break;
            }
            case CWIID_MESG_IR: {
                //cwiid_ir(&aMsgArray[i].ir_mesg);
                break;
            }
            case CWIID_MESG_NUNCHUK: {
                //cwiid_nunchuk(&aMsgArray[i].nunchuk_mesg);
                break;
            }
            case CWIID_MESG_CLASSIC: {
                //cwiid_classic(&aMsgArray[i].classic_mesg);
                break;
            }
            case CWIID_MESG_MOTIONPLUS: {
                //cwiid_motionplus(&aMsgArray[i].motionplus_mesg);
                break;
            }
            case CWIID_MESG_ERROR: {
                //menuDisconnect_activate();
                break;
            }
            default: {
                break;
            }
        }
    }

    gdk_flush();
    gdk_threads_leave();
}

void Wiimote::StatusCallback(struct cwiid_status_mesg *aMsg)
{
    unsigned int batteryLevel = (unsigned int) (100.0 * aMsg->battery /
                                                CWIID_BATTERY_MAX);

    ULOG_DEBUG_F("battery level: %d", batteryLevel);

    //handler->NotifyBattery(batteryLevel);

    IWiimoteObserver::WiimoteExtension ext;

    switch (aMsg->ext_type) {
        case CWIID_EXT_NONE: {
            ext = IWiimoteObserver::ENone;
            break;
        }
        case CWIID_EXT_NUNCHUK: {
            ext = IWiimoteObserver::ENunchuck;
            break;
        }
        case CWIID_EXT_CLASSIC: {
            ext = IWiimoteObserver::EClassic;
            break;
        }
        case CWIID_EXT_BALANCE: {
            ext = IWiimoteObserver::EBalance;
            break;
        }
        case CWIID_EXT_MOTIONPLUS: {
            ext = IWiimoteObserver::EMotionPlus;
            break;
        }
        case CWIID_EXT_UNKNOWN: {
            ext = IWiimoteObserver::EUnknown;
            break;
        }
        default: {
            ULOG_DEBUG_F("Unhandled extenstion type");
            break;
        }
    }

    //handler->NotifyExtension(ext);
}

void Wiimote::ButtonCallback(struct cwiid_btn_mesg *aMsg)
{
    if (aMsg->buttons & CWIID_BTN_UP) {
        NotifyButtonDown(IWiimoteObserver::EUp);
    } else {
        NotifyButtonUp(IWiimoteObserver::EUp);
    }

    if (aMsg->buttons & CWIID_BTN_DOWN) {
        NotifyButtonDown(IWiimoteObserver::EDown);
    } else {
        NotifyButtonUp(IWiimoteObserver::EDown);
    }

    if (aMsg->buttons & CWIID_BTN_LEFT) {
        NotifyButtonDown(IWiimoteObserver::ELeft);
    } else {
        NotifyButtonUp(IWiimoteObserver::ELeft);
    }

    if (aMsg->buttons & CWIID_BTN_RIGHT) {
        NotifyButtonDown(IWiimoteObserver::ERight);
    } else {
        NotifyButtonUp(IWiimoteObserver::ERight);
    }

    if (aMsg->buttons & CWIID_BTN_A) {
        NotifyButtonDown(IWiimoteObserver::EA);
    } else {
        NotifyButtonUp(IWiimoteObserver::EA);
    }

    if (aMsg->buttons & CWIID_BTN_B) {
        NotifyButtonDown(IWiimoteObserver::EB);
    } else {
        NotifyButtonUp(IWiimoteObserver::EB);
    }

    if (aMsg->buttons & CWIID_BTN_MINUS) {
        NotifyButtonDown(IWiimoteObserver::EMinus);
    } else {
        NotifyButtonUp(IWiimoteObserver::EMinus);
    }

    if (aMsg->buttons & CWIID_BTN_PLUS) {
        NotifyButtonDown(IWiimoteObserver::EPlus);
    } else {
        NotifyButtonUp(IWiimoteObserver::EPlus);
    }

    if (aMsg->buttons & CWIID_BTN_HOME) {
        NotifyButtonDown(IWiimoteObserver::EHome);
    } else {
        NotifyButtonUp(IWiimoteObserver::EHome);
    }

    if (aMsg->buttons & CWIID_BTN_1) {
        NotifyButtonDown(IWiimoteObserver::E1);
    } else {
        NotifyButtonUp(IWiimoteObserver::E1);
    }

    if (aMsg->buttons & CWIID_BTN_2) {
        NotifyButtonDown(IWiimoteObserver::E2);
    } else {
        NotifyButtonUp(IWiimoteObserver::E2);
    }
}

void Wiimote::NotifyButtonDown(IWiimoteObserver::WiimoteButton aButton)
{
    list<const IWiimoteObserver*>::iterator i;
    for (i = mObservers.begin(); i != mObservers.end(); ++i) {
        IWiimoteObserver* observer = (IWiimoteObserver*)*i;
        observer->ButtonDown(aButton);
    }
}

void Wiimote::NotifyButtonUp(IWiimoteObserver::WiimoteButton aButton)
{
    list<const IWiimoteObserver*>::iterator i;
    for (i = mObservers.begin(); i != mObservers.end(); ++i) {
        IWiimoteObserver* observer = (IWiimoteObserver*)*i;
        observer->ButtonUp(aButton);
    }
}

void Wiimote::NotifyConnectionStatus(IWiimoteObserver::ConnStatus aStatus)
{
    list<const IWiimoteObserver*>::iterator i;
    for (i = mObservers.begin(); i != mObservers.end(); ++i) {
        IWiimoteObserver* observer = (IWiimoteObserver*)*i;
        observer->ConnectionStatus(this, aStatus);
    }
}
