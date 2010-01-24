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

#ifndef __MOTIONSENSORFRAME_H_
#define __MOTIONSENSORFRAME_H_

#include <hildonmm.h>

#include "wiimoteobserver.h"

class WiimoteHandler;

class MotionSensorFrame : public Gtk::Frame,
                          public IWiimoteObserver
{
public:

    MotionSensorFrame();

    virtual ~MotionSensorFrame();

    void ConnectionStatus(Wiimote* aWiimote, ConnStatus aStatus);

    void MotionData(unsigned int aXAcc,
                    unsigned int aYAcc,
                    unsigned int aZAcc,
                    double aAcc,
                    double aRoll,
                    double aPitch);

private:

    void AddAxis(Gtk::Label& aLabel,
                 Gtk::Label& aValue,
                 Gtk::ProgressBar& aBar,
                 unsigned int aRow);

    void AddLabelAndValue(Gtk::Label& aLabel,
                          Gtk::Label& aValue,
                          unsigned int aRow);

private:
    // Child widgets
    Gtk::Alignment mAlign;
    Gtk::Table mTable;

    Gtk::Label mXLabel;
    Gtk::Label mXValue;
    Gtk::ProgressBar mXProgr;

    Gtk::Label mYLabel;
    Gtk::Label mYValue;
    Gtk::ProgressBar mYProgr;

    Gtk::Label mZLabel;
    Gtk::Label mZValue;
    Gtk::ProgressBar mZProgr;

    Gtk::Label mAccLabel;
    Gtk::Label mAccValue;

    Gtk::Label mRollLabel;
    Gtk::Label mRollValue;

    Gtk::Label mPitchLabel;
    Gtk::Label mPitchValue;

    WiimoteHandler *mHandler;
};

#endif // __MOTIONSENSORFRAME_H_
