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

#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include <hildonmm.h>

class MainWindow : public Hildon::StackableWindow
{
public:
    MainWindow();
    virtual ~MainWindow();

protected:

private:

    // Child widgets
    Gtk::Frame button_frame;
    Gtk::Alignment alignment;
    Gtk::Table table1;

    Gtk::Alignment alignment2;
    Gtk::EventBox ev2;
    Gtk::Label label2;

    Gtk::Alignment alignment1;
    Gtk::EventBox ev1;
    Gtk::Label label1;
    
    //Gtk::Label m_label;
};

#endif // __MAINWINDOW_H_
