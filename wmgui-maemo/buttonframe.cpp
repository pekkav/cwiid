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

#include "common.h"
#include "buttonframe.h"
#include "wiimotehandler.h"

ButtonFrame::ButtonFrame() : Gtk::Frame("Buttons"),
                             mTable(8, 3, false),
                             mBLabel("B"),
                             mUpLabel("Up"),
                             mLeftLabel("Left"),
                             mRightLabel("Right"),
                             mDownLabel("Down"),
                             mALabel("A"),
                             mMinusLabel("-"),
                             mHomeLabel("Home"),
                             mPlusLabel("+"),
                             m1Label("1"),
                             m2Label("2"),
                             mHandler(NULL)
{
    // B button
    AddButton(mBLabel, mBEv, mBAlign,
              1, 2, 0, 1,
              (Gtk::AttachOptions) (0));

    // Up button
    AddButton(mUpLabel, mUpEv, mUpAlign,
              1, 2, 1, 2,
              (Gtk::AttachOptions) (0));

    // Left button
    AddButton(mLeftLabel, mLeftEv, mLeftAlign,
              0, 1, 2, 3);

    // Right button
    AddButton(mRightLabel, mRightEv, mRightAlign,
              2, 3, 2, 3,
              (Gtk::AttachOptions) (Gtk::EXPAND | Gtk::FILL),
              (Gtk::AttachOptions) (0));

    // Down button
    AddButton(mDownLabel, mDownEv, mDownAlign,
              1, 2, 3, 4,
              (Gtk::AttachOptions) (0));

    // A button
    AddButton(mALabel, mAEv, mAAlign,
              1, 2, 4, 5,
              (Gtk::AttachOptions) (0));

    // - button
    AddButton(mMinusLabel, mMinusEv, mMinusAlign,
              0, 1, 5, 6);

    // Home button
    AddButton(mHomeLabel, mHomeEv, mHomeAlign,
              1, 2, 5, 6);

    // + button
    AddButton(mPlusLabel, mPlusEv, mPlusAlign,
              2, 3, 5, 6,
              (Gtk::AttachOptions) (Gtk::EXPAND | Gtk::FILL),
              (Gtk::AttachOptions) (0));

    // 1 button
    AddButton(m1Label, m1Ev, m1Align,
              1, 2, 6, 7,
              (Gtk::AttachOptions) (0));

    // 2 button
    AddButton(m2Label, m2Ev, m2Align,
              1, 2, 7, 8,
              (Gtk::AttachOptions) (0));

    mTable.show();
    add(mTable);

    mHandler = WiimoteHandler::GetInstance();
    mHandler->AddObserver(this);
}

ButtonFrame::~ButtonFrame()
{
    mHandler->RemoveObserver(this);
    WiimoteHandler::Release();
}

void ButtonFrame::ConnectionStatus(ConnStatus aStatus)
{
    bool sensitive = false;
    if (aStatus == EConnected) {
        sensitive = true;
    }

    ULOG_DEBUG_F("sensitive: %d", sensitive);

    mBLabel.set_sensitive(sensitive);
    mUpLabel.set_sensitive(sensitive);
    mLeftLabel.set_sensitive(sensitive);
    mRightLabel.set_sensitive(sensitive);
    mDownLabel.set_sensitive(sensitive);
    mALabel.set_sensitive(sensitive);
    mMinusLabel.set_sensitive(sensitive);
    mHomeLabel.set_sensitive(sensitive);
    mPlusLabel.set_sensitive(sensitive);
    m1Label.set_sensitive(sensitive);
    m2Label.set_sensitive(sensitive);
}

void ButtonFrame::ButtonDown(WiimoteButton aButton)
{
    ULOG_DEBUG_F("%d", aButton);

    Gdk::Color green("green");

    switch (aButton) {
        case EUp: {
            mUpEv.modify_bg(Gtk::STATE_NORMAL, green);
            break;
        }
        case EDown: {
            mDownEv.modify_bg(Gtk::STATE_NORMAL, green);
            break;
        }
        case ERight: {
            mRightEv.modify_bg(Gtk::STATE_NORMAL, green);
            break;
        }
        case ELeft: {
            mLeftEv.modify_bg(Gtk::STATE_NORMAL, green);
            break;
        }
        case EA: {
            mAEv.modify_bg(Gtk::STATE_NORMAL, green);
            break;
        }
        case EB: {
            mBEv.modify_bg(Gtk::STATE_NORMAL, green);
            break;
        }
        case EHome: {
            mHomeEv.modify_bg(Gtk::STATE_NORMAL, green);
            break;
        }
        case EPlus: {
            mPlusEv.modify_bg(Gtk::STATE_NORMAL, green);
            break;
        }
        case EMinus: {
            mMinusEv.modify_bg(Gtk::STATE_NORMAL, green);
            break;
        }
        case E1: {
            m1Ev.modify_bg(Gtk::STATE_NORMAL, green);
            break;
        }
        case E2: {
            m2Ev.modify_bg(Gtk::STATE_NORMAL, green);
            break;
        }
        default: {
            ULOG_DEBUG_F("Unhandled button: %d", aButton);
            break;
        }
    }
}

void ButtonFrame::ButtonUp(WiimoteButton aButton)
{
    ULOG_DEBUG_F("%d", aButton);

    switch (aButton) {
        case EUp: {
            mUpEv.unset_bg(Gtk::STATE_NORMAL);
            break;
        }
        case EDown: {
            mDownEv.unset_bg(Gtk::STATE_NORMAL);
            break;
        }
        case ERight: {
            mRightEv.unset_bg(Gtk::STATE_NORMAL);
            break;
        }
        case ELeft: {
            mLeftEv.unset_bg(Gtk::STATE_NORMAL);
            break;
        }
        case EA: {
            mAEv.unset_bg(Gtk::STATE_NORMAL);
            break;
        }
        case EB: {
            mBEv.unset_bg(Gtk::STATE_NORMAL);
            break;
        }
        case EHome: {
            mHomeEv.unset_bg(Gtk::STATE_NORMAL);
            break;
        }
        case EPlus: {
            mPlusEv.unset_bg(Gtk::STATE_NORMAL);
            break;
        }
        case EMinus: {
            mMinusEv.unset_bg(Gtk::STATE_NORMAL);
            break;
        }
        case E1: {
            m1Ev.unset_bg(Gtk::STATE_NORMAL);
            break;
        }
        case E2: {
            m2Ev.unset_bg(Gtk::STATE_NORMAL);
            break;
        }
        default: {
            ULOG_DEBUG_F("Unhandled button: %d", aButton);
            break;
        }
    }
}

void ButtonFrame::AddButton(Gtk::Widget& aLabel, Gtk::EventBox& aEv, Gtk::Alignment& aAlign,
                            unsigned int aLeftAttach, unsigned int aRightAttach,
                            unsigned int aTopAttach, unsigned int aBottomAttach,
                            Gtk::AttachOptions aXOptions /*= Gtk::FILL | Gtk::EXPAND*/,
                            Gtk::AttachOptions aYOptions /*= Gtk::FILL | Gtk::EXPAND*/)
{
    // Set initially insensitive
    aLabel.set_sensitive(false);
    // Show the label
    aLabel.show();
    // Add label to event box
    aEv.add(aLabel);
    aEv.show();
    // Add event box to align
    aAlign.add(aEv);
    aAlign.show();
    // Attach align to table
    mTable.attach(aAlign,
                  aLeftAttach, aRightAttach,
                  aTopAttach, aBottomAttach,
                  aXOptions,
                  aYOptions);
}
