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
}

void MotionSensorFrame::MotionData(unsigned int aXAcc,
                                   unsigned int aYAcc,
                                   unsigned int aZAcc,
                                   double aAcc,
                                   double aRoll,
                                   double aPitch)
{
    ULOG_DEBUG_F("%d, %d, %d, %f, %f, %f",
                 aXAcc, aYAcc, aZAcc, aAcc, aRoll, aPitch);
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
    aValue.set_size_request(35, -1);
    aValue.show();
    mTable.attach(aValue, 1, 2, aRow, aRow + 1,
                  (Gtk::AttachOptions) (0),
                  (Gtk::AttachOptions) (0));

    aProgrBar.set_sensitive(false);
    aProgrBar.show();
    mTable.attach(aProgrBar, 2, 3, aRow, aRow + 1,
                  (Gtk::AttachOptions) (Gtk::EXPAND | Gtk::FILL),
                  (Gtk::AttachOptions) (0));
}

void MotionSensorFrame::AddLabelAndValue(Gtk::Label& aLabel,
                                         Gtk::Label& aValue,
                                         unsigned int aRow)
{
    aLabel.set_sensitive(false);
    aLabel.show();
    mTable.attach(aLabel, 0, 1, aRow, aRow + 1,
                  (Gtk::AttachOptions) (0),
                  (Gtk::AttachOptions) (0));

    aValue.set_sensitive(false);
    aValue.show();
    mTable.attach(aValue, 1, 2, aRow, aRow + 1,
                  (Gtk::AttachOptions) (0),
                  (Gtk::AttachOptions) (0));
}

/*
  frame2 = gtk_frame_new (NULL);
  gtk_widget_set_name (frame2, "frame2");
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (hbox1), frame2, TRUE, TRUE, 3);

  alignment2 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_set_name (alignment2, "alignment2");
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (frame2), alignment2);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment2), 0, 0, 12, 0);

  hbox2 = gtk_hbox_new (TRUE, 6);
  gtk_widget_set_name (hbox2, "hbox2");
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  table4 = gtk_table_new (6, 3, FALSE);
  gtk_widget_set_name (table4, "table4");
  gtk_widget_show (table4);
  gtk_box_pack_start (GTK_BOX (hbox2), table4, TRUE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table4), 3);

  btnBeep = gtk_button_new_with_mnemonic ("Beep");
  gtk_widget_set_name (btnBeep, "btnBeep");
  gtk_table_attach (GTK_TABLE (table4), btnBeep, 2, 3, 4, 5,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  lblAccVal = gtk_label_new ("0");
  gtk_widget_set_name (lblAccVal, "lblAccVal");
  gtk_widget_show (lblAccVal);
  gtk_table_attach (GTK_TABLE (table4), lblAccVal, 1, 2, 3, 4,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (lblAccVal, 35, -1);
  gtk_widget_set_sensitive (lblAccVal, FALSE);
  gtk_misc_set_alignment (GTK_MISC (lblAccVal), 0, 0.5);

  lblAcc = gtk_label_new ("Acc:");
  gtk_widget_set_name (lblAcc, "lblAcc");
  gtk_widget_show (lblAcc);
  gtk_table_attach (GTK_TABLE (table4), lblAcc, 0, 1, 3, 4,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (lblAcc, FALSE);
  gtk_misc_set_alignment (GTK_MISC (lblAcc), 0, 0.5);

  lblRollVal = gtk_label_new ("0");
  gtk_widget_set_name (lblRollVal, "lblRollVal");
  gtk_widget_show (lblRollVal);
  gtk_table_attach (GTK_TABLE (table4), lblRollVal, 1, 2, 4, 5,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (lblRollVal, 35, -1);
  gtk_widget_set_sensitive (lblRollVal, FALSE);
  gtk_misc_set_alignment (GTK_MISC (lblRollVal), 0, 0.5);

  lblRoll = gtk_label_new ("Roll:");
  gtk_widget_set_name (lblRoll, "lblRoll");
  gtk_widget_show (lblRoll);
  gtk_table_attach (GTK_TABLE (table4), lblRoll, 0, 1, 4, 5,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (lblRoll, FALSE);
  gtk_label_set_justify (GTK_LABEL (lblRoll), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment (GTK_MISC (lblRoll), 0, 0.5);

  lblPitchVal = gtk_label_new ("0");
  gtk_widget_set_name (lblPitchVal, "lblPitchVal");
  gtk_widget_show (lblPitchVal);
  gtk_table_attach (GTK_TABLE (table4), lblPitchVal, 1, 2, 5, 6,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (lblPitchVal, 35, -1);
  gtk_widget_set_sensitive (lblPitchVal, FALSE);
  gtk_misc_set_alignment (GTK_MISC (lblPitchVal), 0, 0.5);

  lblPitch = gtk_label_new ("Pitch:");
  gtk_widget_set_name (lblPitch, "lblPitch");
  gtk_widget_show (lblPitch);
  gtk_table_attach (GTK_TABLE (table4), lblPitch, 0, 1, 5, 6,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (lblPitch, FALSE);
  gtk_label_set_justify (GTK_LABEL (lblPitch), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment (GTK_MISC (lblPitch), 0, 0.5);

  lblAccZVal = gtk_label_new ("0");
  gtk_widget_set_name (lblAccZVal, "lblAccZVal");
  gtk_widget_show (lblAccZVal);
  gtk_table_attach (GTK_TABLE (table4), lblAccZVal, 1, 2, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (lblAccZVal, 35, -1);
  gtk_widget_set_sensitive (lblAccZVal, FALSE);
  gtk_misc_set_alignment (GTK_MISC (lblAccZVal), 0, 0.5);

  lblAccZ = gtk_label_new ("Z:");
  gtk_widget_set_name (lblAccZ, "lblAccZ");
  gtk_widget_show (lblAccZ);
  gtk_table_attach (GTK_TABLE (table4), lblAccZ, 0, 1, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (lblAccZ, FALSE);
  gtk_label_set_justify (GTK_LABEL (lblAccZ), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment (GTK_MISC (lblAccZ), 0, 0.5);

  lblAccYVal = gtk_label_new ("0");
  gtk_widget_set_name (lblAccYVal, "lblAccYVal");
  gtk_widget_show (lblAccYVal);
  gtk_table_attach (GTK_TABLE (table4), lblAccYVal, 1, 2, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (lblAccYVal, 35, -1);
  gtk_widget_set_sensitive (lblAccYVal, FALSE);
  gtk_misc_set_alignment (GTK_MISC (lblAccYVal), 0, 0.5);

  lblAccY = gtk_label_new ("Y:");
  gtk_widget_set_name (lblAccY, "lblAccY");
  gtk_widget_show (lblAccY);
  gtk_table_attach (GTK_TABLE (table4), lblAccY, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (lblAccY, FALSE);
  gtk_label_set_justify (GTK_LABEL (lblAccY), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment (GTK_MISC (lblAccY), 0, 0.5);

  lblAccXVal = gtk_label_new ("0");
  gtk_widget_set_name (lblAccXVal, "lblAccXVal");
  gtk_widget_show (lblAccXVal);
  gtk_table_attach (GTK_TABLE (table4), lblAccXVal, 1, 2, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (lblAccXVal, 35, -1);
  gtk_widget_set_sensitive (lblAccXVal, FALSE);
  gtk_misc_set_alignment (GTK_MISC (lblAccXVal), 0, 0.5);

  lblAccX = gtk_label_new ("X:");
  gtk_widget_set_name (lblAccX, "lblAccX");
  gtk_widget_show (lblAccX);
  gtk_table_attach (GTK_TABLE (table4), lblAccX, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (lblAccX, FALSE);
  gtk_label_set_justify (GTK_LABEL (lblAccX), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment (GTK_MISC (lblAccX), 0, 0.5);

  progAccZ = gtk_progress_bar_new ();
  gtk_widget_set_name (progAccZ, "progAccZ");
  gtk_widget_show (progAccZ);
  gtk_table_attach (GTK_TABLE (table4), progAccZ, 2, 3, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (progAccZ, FALSE);

  progAccY = gtk_progress_bar_new ();
  gtk_widget_set_name (progAccY, "progAccY");
  gtk_widget_show (progAccY);
  gtk_table_attach (GTK_TABLE (table4), progAccY, 2, 3, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (progAccY, FALSE);

  progAccX = gtk_progress_bar_new ();
  gtk_widget_set_name (progAccX, "progAccX");
  gtk_widget_show (progAccX);
  gtk_table_attach (GTK_TABLE (table4), progAccX, 2, 3, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (progAccX, FALSE);
*/
