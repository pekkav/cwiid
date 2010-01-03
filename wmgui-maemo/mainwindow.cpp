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

#include <iostream>
#include <hildon/hildon.h>

#include "common.h"
#include "mainwindow.h"

MainWindow::MainWindow() : alignment(0.5, 0.5, 1, 1),
                           table1(6, 3, TRUE),
                           alignment2(0.5, 0.5, 1, 1),
                           label2("2"),
                           alignment1(0.5, 0.5, 1, 1),
                           label1("1")
{
    // Frame 1,  buttons
    button_frame.set_name("button_frame");
    button_frame.show();

    alignment.set_name("alignment");
    alignment.set_padding(0, 0, 12, 0);
    alignment.show();
    button_frame.add(alignment);

    table1.set_name("table1");
    table1.show();
    alignment.add(table1);

    // 2
    alignment2.set_name("alignment2");
    alignment2.show();
    table1.attach(alignment2, 2, 3, 5, 6,
                  (Gtk::AttachOptions)0,
                  (Gtk::AttachOptions)0,
                  0, 0);

    ev2.set_name("ev2");
    ev2.show();
    alignment2.add(ev2);

    label2.set_name("label2");
    label2.show();
    ev2.add(label2);

    // 1
    alignment1.set_name("alignmen1");
    alignment1.show();
    table1.attach(alignment1, 0, 1, 5, 6,
                  (Gtk::AttachOptions)0,
                  (Gtk::AttachOptions)(Gtk::EXPAND | Gtk::FILL),
                  0, 0);

    ev1.set_name("ev1");
    ev1.show();
    alignment1.add(ev1);

    label1.set_name("label1");
    label1.show();
    ev1.add(label1);

    /*
    m_label.set_text("Label");
    m_label.show();
    m_button_frame.add(m_label);
    */

    // Add widgets to the window
    add(button_frame);

    // Make everything visible
    show_all();

    ULOG_DEBUG_F("MainWindow created");
    
    /*
  alignment12 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_set_name (alignment12, "alignment12");
  gtk_widget_show (alignment12);
  gtk_table_attach (GTK_TABLE (table1), alignment12, 2, 3, 4, 5,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  evPlus = gtk_event_box_new ();
  gtk_widget_set_name (evPlus, "evPlus");
  gtk_widget_show (evPlus);
  gtk_container_add (GTK_CONTAINER (alignment12), evPlus);

  lblPlus = gtk_label_new ("+");
  gtk_widget_set_name (lblPlus, "lblPlus");
  gtk_widget_show (lblPlus);
  gtk_container_add (GTK_CONTAINER (evPlus), lblPlus);

  alignment11 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_set_name (alignment11, "alignment11");
  gtk_widget_show (alignment11);
  gtk_table_attach (GTK_TABLE (table1), alignment11, 1, 2, 4, 5,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  evHome = gtk_event_box_new ();
  gtk_widget_set_name (evHome, "evHome");
  gtk_widget_show (evHome);
  gtk_container_add (GTK_CONTAINER (alignment11), evHome);

  lblHome = gtk_label_new ("Home");
  gtk_widget_set_name (lblHome, "lblHome");
  gtk_widget_show (lblHome);
  gtk_container_add (GTK_CONTAINER (evHome), lblHome);

  alignment10 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_set_name (alignment10, "alignment10");
  gtk_widget_show (alignment10);
  gtk_table_attach (GTK_TABLE (table1), alignment10, 0, 1, 4, 5,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  evMinus = gtk_event_box_new ();
  gtk_widget_set_name (evMinus, "evMinus");
  gtk_widget_show (evMinus);
  gtk_container_add (GTK_CONTAINER (alignment10), evMinus);

  lblMinus = gtk_label_new ("-");
  gtk_widget_set_name (lblMinus, "lblMinus");
  gtk_widget_show (lblMinus);
  gtk_container_add (GTK_CONTAINER (evMinus), lblMinus);

  alignment9 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_set_name (alignment9, "alignment9");
  gtk_widget_show (alignment9);
  gtk_table_attach (GTK_TABLE (table1), alignment9, 2, 3, 3, 4,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  evB = gtk_event_box_new ();
  gtk_widget_set_name (evB, "evB");
  gtk_widget_show (evB);
  gtk_container_add (GTK_CONTAINER (alignment9), evB);

  lblB = gtk_label_new ("B");
  gtk_widget_set_name (lblB, "lblB");
  gtk_widget_show (lblB);
  gtk_container_add (GTK_CONTAINER (evB), lblB);

  alignment7 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_set_name (alignment7, "alignment7");
  gtk_widget_show (alignment7);
  gtk_table_attach (GTK_TABLE (table1), alignment7, 0, 1, 3, 4,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  evA = gtk_event_box_new ();
  gtk_widget_set_name (evA, "evA");
  gtk_widget_show (evA);
  gtk_container_add (GTK_CONTAINER (alignment7), evA);

  lblA = gtk_label_new ("A");
  gtk_widget_set_name (lblA, "lblA");
  gtk_widget_show (lblA);
  gtk_container_add (GTK_CONTAINER (evA), lblA);

  alignment6 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_set_name (alignment6, "alignment6");
  gtk_widget_show (alignment6);
  gtk_table_attach (GTK_TABLE (table1), alignment6, 1, 2, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  evDown = gtk_event_box_new ();
  gtk_widget_set_name (evDown, "evDown");
  gtk_widget_show (evDown);
  gtk_container_add (GTK_CONTAINER (alignment6), evDown);

  lblDown = gtk_label_new ("Down");
  gtk_widget_set_name (lblDown, "lblDown");
  gtk_widget_show (lblDown);
  gtk_container_add (GTK_CONTAINER (evDown), lblDown);

  alignment5 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_set_name (alignment5, "alignment5");
  gtk_widget_show (alignment5);
  gtk_table_attach (GTK_TABLE (table1), alignment5, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  evRight = gtk_event_box_new ();
  gtk_widget_set_name (evRight, "evRight");
  gtk_widget_show (evRight);
  gtk_container_add (GTK_CONTAINER (alignment5), evRight);

  lblRight = gtk_label_new ("Right");
  gtk_widget_set_name (lblRight, "lblRight");
  gtk_widget_show (lblRight);
  gtk_container_add (GTK_CONTAINER (evRight), lblRight);

  alignment4 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_set_name (alignment4, "alignment4");
  gtk_widget_show (alignment4);
  gtk_table_attach (GTK_TABLE (table1), alignment4, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  evLeft = gtk_event_box_new ();
  gtk_widget_set_name (evLeft, "evLeft");
  gtk_widget_show (evLeft);
  gtk_container_add (GTK_CONTAINER (alignment4), evLeft);

  lblLeft = gtk_label_new ("Left");
  gtk_widget_set_name (lblLeft, "lblLeft");
  gtk_widget_show (lblLeft);
  gtk_container_add (GTK_CONTAINER (evLeft), lblLeft);

  alignment3 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_set_name (alignment3, "alignment3");
  gtk_widget_show (alignment3);
  gtk_table_attach (GTK_TABLE (table1), alignment3, 1, 2, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  evUp = gtk_event_box_new ();
  gtk_widget_set_name (evUp, "evUp");
  gtk_widget_show (evUp);
  gtk_container_add (GTK_CONTAINER (alignment3), evUp);

  lblUp = gtk_label_new ("Up");
  gtk_widget_set_name (lblUp, "lblUp");
  gtk_widget_show (lblUp);
  gtk_container_add (GTK_CONTAINER (evUp), lblUp);

  label1 = gtk_label_new ("<b>Buttons</b>");
  gtk_widget_set_name (label1, "label1");
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
*/
}

MainWindow::~MainWindow()
{

}
