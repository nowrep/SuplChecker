/*  SuplChecker - simple program to check a teacher's absencies at the school
    Copyright (C) 2010-2011  nowrep

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QByteArray>
#include <QPixmap>
#include <QBuffer>
#include <QFile>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>

QByteArray sc_pixmapToByteArray(const QPixmap &pix, const char* type = "PNG");
QByteArray sc_readAllFileContents(const QString &filename);

void sc_centerWidgetOnScreen(QWidget* w);

#endif // GLOBALFUNCTIONS_H
