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

#include <gdk/gdk.h>

#include "common.h"
#include "wiimotehandler.h"

WiimoteHandler* WiimoteHandler::mInstance = NULL;
unsigned int WiimoteHandler::mRefCount = 0;

// Wiimote callback
cwiid_mesg_callback_t cwiid_callback;

// static
WiimoteHandler* WiimoteHandler::GetInstance()
{
    if (!mInstance) {
        mInstance = new WiimoteHandler();
    }

    mRefCount++;
    return mInstance;
}

// static
void WiimoteHandler::Release()
{
    if (mRefCount) {
        mRefCount--;
    }

    if (mRefCount == 0 && mInstance) {
        delete mInstance;
        mInstance = NULL;
    }
}

bdaddr_t* WiimoteHandler::GetBdAddr() const
{
    return mBdAddr;
}

void WiimoteHandler::SetBdAddr(const bdaddr_t* aBdAddr)
{
    bacpy(mBdAddr, aBdAddr);
}

void WiimoteHandler::Connect()
{
    bool resetAddr = false;

    if (bacmp(mBdAddr, BDADDR_ANY) == 0) {
        resetAddr = true;
    }

    ULOG_DEBUG_F("resetAddr: %d", resetAddr);

    NotifyConnectionStatus(IWiimoteObserver::EConnecting);

    if ((mWiimote = cwiid_open(mBdAddr, CWIID_FLAG_MESG_IFC)) == NULL) {
        ULOG_DEBUG_F("Unable to connect");
        //message(GTK_MESSAGE_ERROR, "Unable to connect", GTK_WINDOW(winMain));
        //status("No connection");
        NotifyConnectionStatus(IWiimoteObserver::EConnectionError);
        NotifyConnectionStatus(IWiimoteObserver::ENotConnected);
    } else if (cwiid_set_mesg_callback(mWiimote, &cwiid_callback)) {
        ULOG_DEBUG_F("Error setting callback");
        //message(GTK_MESSAGE_ERROR, "Error setting callback",
        //        GTK_WINDOW(winMain));
        if (cwiid_close(mWiimote)) {
            //message(GTK_MESSAGE_ERROR, "Error on disconnect",
            //        GTK_WINDOW(winMain));
        }
        mWiimote = NULL;
        //status("No connection");
        NotifyConnectionStatus(IWiimoteObserver::EConnectionError);
        NotifyConnectionStatus(IWiimoteObserver::ENotConnected);
    } else {
        ULOG_DEBUG_F("Connected");
        //status("Connected");
        /*
        if (cwiid_get_acc_cal(mWiimote, CWIID_EXT_NONE, &wm_cal)) {
            message(GTK_MESSAGE_ERROR, "Unable to retrieve accelerometer "
                    "calibration", GTK_WINDOW(winMain));
        }
        set_gui_state();
        set_report_mode();
        cwiid_enable(wiimote, CWIID_FLAG_MOTIONPLUS);
        cwiid_request_status(wiimote);
        */
        uint8_t rpt_mode;    
        rpt_mode = CWIID_RPT_STATUS | CWIID_RPT_BTN;
        if (cwiid_set_rpt_mode(mWiimote, rpt_mode)) {
            ULOG_DEBUG_F("Error setting report mode");
        }
        NotifyConnectionStatus(IWiimoteObserver::EConnected);
    }

    if (resetAddr) {
        bacpy(mBdAddr, BDADDR_ANY);
    }
}

void WiimoteHandler::AddObserver(const IWiimoteObserver* aObserver)
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

void WiimoteHandler::RemoveObserver(const IWiimoteObserver* aObserver)
{
    list<const IWiimoteObserver*>::iterator i;
    for (i = mObservers.begin(); i != mObservers.end(); ++i) {
        if (*i == aObserver) {
            mObservers.erase(i);
            return;
        }
    }
}

void WiimoteHandler::NotifyConnectionStatus(IWiimoteObserver::ConnStatus aStatus)
{
    list<const IWiimoteObserver*>::iterator i;
    for (i = mObservers.begin(); i != mObservers.end(); ++i) {
        IWiimoteObserver* observer = (IWiimoteObserver*)*i;
        observer->ConnectionStatus(aStatus);
    }
}

void WiimoteHandler::NotifyButtonDown(IWiimoteObserver::WiimoteButton aButton)
{
    list<const IWiimoteObserver*>::iterator i;
    for (i = mObservers.begin(); i != mObservers.end(); ++i) {
        IWiimoteObserver* observer = (IWiimoteObserver*)*i;
        observer->ButtonDown(aButton);
    }
}

void WiimoteHandler::NotifyButtonUp(IWiimoteObserver::WiimoteButton aButton)
{
    list<const IWiimoteObserver*>::iterator i;
    for (i = mObservers.begin(); i != mObservers.end(); ++i) {
        IWiimoteObserver* observer = (IWiimoteObserver*)*i;
        observer->ButtonUp(aButton);
    }
}

WiimoteHandler::WiimoteHandler() : mWiimote(NULL)
{
    mBdAddr = (bdaddr_t*) (bt_malloc(sizeof(bdaddr_t)));
}

WiimoteHandler::~WiimoteHandler()
{
    if (mBdAddr) {
        bt_free(mBdAddr);
    }

    if (mWiimote) {
        cwiid_close(mWiimote);
    }
}

void cwiid_btn(struct cwiid_btn_mesg *mesg)
{
    WiimoteHandler *handler = WiimoteHandler::GetInstance();

    if (mesg->buttons & CWIID_BTN_UP) {
        handler->NotifyButtonDown(IWiimoteObserver::EUp);
    } else {
        handler->NotifyButtonUp(IWiimoteObserver::EUp);
    }

    if (mesg->buttons & CWIID_BTN_DOWN) {
        handler->NotifyButtonDown(IWiimoteObserver::EDown);
    } else {
        handler->NotifyButtonUp(IWiimoteObserver::EDown);
    }

    if (mesg->buttons & CWIID_BTN_LEFT) {
        handler->NotifyButtonDown(IWiimoteObserver::ELeft);
    } else {
        handler->NotifyButtonUp(IWiimoteObserver::ELeft);
    }

    if (mesg->buttons & CWIID_BTN_RIGHT) {
        handler->NotifyButtonDown(IWiimoteObserver::ERight);
    } else {
        handler->NotifyButtonUp(IWiimoteObserver::ERight);
    }

    if (mesg->buttons & CWIID_BTN_A) {
        handler->NotifyButtonDown(IWiimoteObserver::EA);
    } else {
        handler->NotifyButtonUp(IWiimoteObserver::EA);
    }

    if (mesg->buttons & CWIID_BTN_B) {
        handler->NotifyButtonDown(IWiimoteObserver::EB);
    } else {
        handler->NotifyButtonUp(IWiimoteObserver::EB);
    }

    if (mesg->buttons & CWIID_BTN_MINUS) {
        handler->NotifyButtonDown(IWiimoteObserver::EMinus);
    } else {
        handler->NotifyButtonUp(IWiimoteObserver::EMinus);
    }

    if (mesg->buttons & CWIID_BTN_PLUS) {
        handler->NotifyButtonDown(IWiimoteObserver::EPlus);
    } else {
        handler->NotifyButtonUp(IWiimoteObserver::EPlus);
    }

    if (mesg->buttons & CWIID_BTN_HOME) {
        handler->NotifyButtonDown(IWiimoteObserver::EHome);
    } else {
        handler->NotifyButtonUp(IWiimoteObserver::EHome);
    }

    if (mesg->buttons & CWIID_BTN_1) {
        handler->NotifyButtonDown(IWiimoteObserver::E1);
    } else {
        handler->NotifyButtonUp(IWiimoteObserver::E1);
    }

    if (mesg->buttons & CWIID_BTN_2) {
        handler->NotifyButtonDown(IWiimoteObserver::E2);
    } else {
        handler->NotifyButtonUp(IWiimoteObserver::E2);
    }

    WiimoteHandler::Release();
}

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count,
                    union cwiid_mesg mesg_array[], struct timespec *timestamp)
{
    int i;
//    char battery[BATTERY_STR_LEN];
//    char *ext_str;
    static enum cwiid_ext_type ext_type = CWIID_EXT_NONE;

    gdk_threads_enter();

    for (i = 0; i < mesg_count; i++) {
        switch (mesg_array[i].type) {
            case CWIID_MESG_STATUS: {
                /*
                snprintf(battery, BATTERY_STR_LEN,"Battery:%d%%",
                        (int) (100.0 * mesg_array[i].status_mesg.battery /
                         CWIID_BATTERY_MAX));
                gtk_statusbar_push(GTK_STATUSBAR(statBattery), 0, battery);
                switch (mesg_array[i].status_mesg.ext_type) {
                    case CWIID_EXT_NONE: {
                        ext_str = "No extension";
                        break;
                    }
                    case CWIID_EXT_NUNCHUK: {
                        ext_str = "Nunchuk";
                        if (ext_type != CWIID_EXT_NUNCHUK) {
                            if (cwiid_get_acc_cal(wiimote, CWIID_EXT_NUNCHUK,
                                                  &nc_cal)) {
                                message(GTK_MESSAGE_ERROR,
                                    "Unable to retrieve accelerometer calibration",
                                    GTK_WINDOW(winMain));
                            }
                        }
                        break;
                    }
                    case CWIID_EXT_CLASSIC: {
                        ext_str = "Classic controller";
                        break;
                    }
                    case CWIID_EXT_MOTIONPLUS: {
                        ext_str = "MotionPlus";
                        break;
                    }
                    case CWIID_EXT_UNKNOWN: {
                        ext_str = "Unknown extension";
                        break;
                    }
                }
                gtk_statusbar_push(GTK_STATUSBAR(statExtension), 0, ext_str);
                clear_nunchuk_widgets();
                clear_classic_widgets();
                clear_motionplus_widgets();
                ext_type = mesg_array[i].status_mesg.ext_type;
                */
                break;
            }
            case CWIID_MESG_BTN: {
                cwiid_btn(&mesg_array[i].btn_mesg);
                break;
            }
            case CWIID_MESG_ACC: {
                //cwiid_acc(&mesg_array[i].acc_mesg);
                break;
            }
            case CWIID_MESG_IR: {
                //cwiid_ir(&mesg_array[i].ir_mesg);
                break;
            }
            case CWIID_MESG_NUNCHUK: {
                //cwiid_nunchuk(&mesg_array[i].nunchuk_mesg);
                break;
            }
            case CWIID_MESG_CLASSIC: {
                //cwiid_classic(&mesg_array[i].classic_mesg);
                break;
            }
            case CWIID_MESG_MOTIONPLUS: {
                //cwiid_motionplus(&mesg_array[i].motionplus_mesg);
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
