/***********************************************************************
 * BeagleBoard-X15 audio DSP lib
 *
 * Author: Henrik Langer (henni19790@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#include "display.hpp"
//#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

Display::Display(QWidget *parent)
    : QMainWindow(parent)
      //ui(new Ui::Display)
{
    //ui->setupUi(this);
    setGeometry(400, 250, 542, 390);
}

Display::~Display()
{

}

void Display::drawGraph(size_t n, void *data){

}
