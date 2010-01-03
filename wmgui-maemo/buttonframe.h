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

#ifndef __BUTTONFRAME_H_
#define __BUTTONFRAME_H_

#include <hildonmm.h>

class ButtonFrame : public Gtk::Frame
{
public:

    ButtonFrame();
    
    virtual ~ButtonFrame();

protected:

private:

    void AddButton(Gtk::Widget& aLabel, Gtk::EventBox& aEv, Gtk::Alignment& aAlign,
                   uint aLeftAttach, uint aRightAttach,
                   uint aTopAttach, uint aBottomAttach,
                   Gtk::AttachOptions aXOptions = Gtk::FILL | Gtk::EXPAND,
                   Gtk::AttachOptions aYOptions = Gtk::FILL | Gtk::EXPAND);

private:

    // Child widgets
    Gtk::Table mTable;

    Gtk::Alignment mBAlign;
    Gtk::EventBox mBEv;
    Gtk::Label mBLabel;

    Gtk::Alignment mUpAlign;
    Gtk::EventBox mUpEv;
    Gtk::Label mUpLabel;

    Gtk::Alignment mLeftAlign;
    Gtk::EventBox mLeftEv;
    Gtk::Label mLeftLabel;

    Gtk::Alignment mRightAlign;
    Gtk::EventBox mRightEv;
    Gtk::Label mRightLabel;

    Gtk::Alignment mDownAlign;
    Gtk::EventBox mDownEv;
    Gtk::Label mDownLabel;

    Gtk::Alignment mAAlign;
    Gtk::EventBox mAEv;
    Gtk::Label mALabel;

    Gtk::Alignment mMinusAlign;
    Gtk::EventBox mMinusEv;
    Gtk::Label mMinusLabel;

    Gtk::Alignment mHomeAlign;
    Gtk::EventBox mHomeEv;
    Gtk::Label mHomeLabel;

    Gtk::Alignment mPlusAlign;
    Gtk::EventBox mPlusEv;
    Gtk::Label mPlusLabel;

    Gtk::Alignment m1Align;
    Gtk::EventBox m1Ev;
    Gtk::Label m1Label;

    Gtk::Alignment m2Align;
    Gtk::EventBox m2Ev;
    Gtk::Label m2Label;
};

#endif // __BUTTONFRAME_H_
