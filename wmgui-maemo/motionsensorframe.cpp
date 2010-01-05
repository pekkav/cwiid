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

#include <hildon/hildon.h>

#include "common.h"
#include "motionsensorframe.h"
#include "wiimotehandler.h"

MotionSensorFrame::MotionSensorFrame() : Gtk::Frame("Motion Sensors"),
                                         mTable(6, 3, false),
                                         mXLabel("X:"),
                                         mXValue("0"),
                                         mYLabel("Y:"),
                                         mYValue("0"),
                                         mZLabel("Z:"),
                                         mZValue("0"),
                                         mAccLabel("Acc:"),
                                         mAccValue("0"),
                                         mRollLabel("Roll:"),
                                         mRollValue("0"),
                                         mPitchLabel("Pitch:"),
                                         mPitchValue("0"),
                                         mHandler(NULL)
{
    unsigned int row = 0;

    AddAxis(mXLabel, mXValue, mXProgr, row);
    row++;

    AddAxis(mYLabel, mYValue, mYProgr, row);
    row++;
    
    AddAxis(mZLabel, mZValue, mZProgr, row);
    row++;

    AddLabelAndValue(mAccLabel, mAccValue, row);
    row++;

    AddLabelAndValue(mRollLabel, mRollValue, row);
    row++;

    AddLabelAndValue(mPitchLabel, mPitchValue, row);

    mTable.show();

    mAlign.show();
    mAlign.set_padding(0, 0, HILDON_MARGIN_DEFAULT, HILDON_MARGIN_DEFAULT);
    mAlign.add(mTable);
    
    add(mAlign);

    mHandler = WiimoteHandler::GetInstance();
    mHandler->AddObserver(this);
}

MotionSensorFrame::~MotionSensorFrame()
{
    mHandler->RemoveObserver(this);
    WiimoteHandler::Release();
}

void MotionSensorFrame::ConnectionStatus(ConnStatus aStatus)
{
    bool sensitive = false;
    if (aStatus == EConnected) {
        sensitive = true;
    }

    ULOG_DEBUG_F("sensitive: %d", sensitive);

    mXLabel.set_sensitive(sensitive);
    mXValue.set_sensitive(sensitive);
    mXProgr.set_sensitive(sensitive);
    mYLabel.set_sensitive(sensitive);
    mYValue.set_sensitive(sensitive);
    mYProgr.set_sensitive(sensitive);
    mZLabel.set_sensitive(sensitive);
    mZValue.set_sensitive(sensitive);
    mZProgr.set_sensitive(sensitive);
    mAccLabel.set_sensitive(sensitive);
    mAccValue.set_sensitive(sensitive);
    mRollLabel.set_sensitive(sensitive);
    mRollValue.set_sensitive(sensitive);
    mPitchLabel.set_sensitive(sensitive);
    mPitchValue.set_sensitive(sensitive);

    if (!sensitive) {
        mXValue.set_text("0");
        mXProgr.set_fraction(0);
        mYValue.set_text("0");
        mYProgr.set_fraction(0);
        mZValue.set_text("0");
        mZProgr.set_fraction(0);
        mAccValue.set_text("0");
        mRollValue.set_text("0");
        mPitchValue.set_text("0");
    }
}

void MotionSensorFrame::MotionData(unsigned int aXAcc,
                                   unsigned int aYAcc,
                                   unsigned int aZAcc,
                                   double aAcc,
                                   double aRoll,
                                   double aPitch)
{
    Glib::ustring xVal = Glib::ustring::compose("%1", aXAcc);
    mXValue.set_text(xVal);
    mXProgr.set_fraction((double)aXAcc / 0xFF);

    Glib::ustring yVal = Glib::ustring::compose("%1", aYAcc);
    mYValue.set_text(yVal);
    mYProgr.set_fraction((double)aYAcc / 0xFF);

    Glib::ustring zVal = Glib::ustring::compose("%1", aZAcc);
    mZValue.set_text(zVal);
    mZProgr.set_fraction((double)aZAcc / 0xFF);

    Glib::ustring accVal = Glib::ustring::compose("%1", aAcc);
    mAccValue.set_text(accVal);

    Glib::ustring rollVal = Glib::ustring::compose("%1", aRoll);
    mRollValue.set_text(rollVal);

    Glib::ustring pitchVal = Glib::ustring::compose("%1", aPitch);
    mPitchValue.set_text(pitchVal);
}

void MotionSensorFrame::AddAxis(Gtk::Label& aLabel,
                                Gtk::Label& aValue,
                                Gtk::ProgressBar& aProgrBar,
                                unsigned int aRow)
{
    aLabel.set_sensitive(false);
    aLabel.set_alignment(Gtk::ALIGN_LEFT);
    aLabel.show();
    mTable.attach(aLabel, 0, 1, aRow, aRow + 1,
                  (Gtk::AttachOptions) (0),
                  (Gtk::AttachOptions) (0));

    aValue.set_sensitive(false);
    aValue.set_alignment(Gtk::ALIGN_LEFT);
    aValue.set_size_request(55, -1);
    aValue.show();
    mTable.attach(aValue, 1, 2, aRow, aRow + 1,
                  (Gtk::AttachOptions) (0),
                  (Gtk::AttachOptions) (0));

    aProgrBar.set_sensitive(false);
    aProgrBar.show();
    mTable.attach(aProgrBar, 2, 3, aRow, aRow + 1,
                  (Gtk::AttachOptions) (0),
                  (Gtk::AttachOptions) (0));
}

void MotionSensorFrame::AddLabelAndValue(Gtk::Label& aLabel,
                                         Gtk::Label& aValue,
                                         unsigned int aRow)
{
    aLabel.set_sensitive(false);
    aLabel.set_alignment(Gtk::ALIGN_LEFT);
    aLabel.show();
    mTable.attach(aLabel, 0, 1, aRow, aRow + 1,
                  (Gtk::AttachOptions) (0),
                  (Gtk::AttachOptions) (0));

    aValue.set_sensitive(false);
    aValue.set_alignment(Gtk::ALIGN_LEFT);
    aValue.set_size_request(55, -1);
    aValue.show();
    mTable.attach(aValue, 1, 2, aRow, aRow + 1,
                  (Gtk::AttachOptions) (0),
                  (Gtk::AttachOptions) (0));
}
