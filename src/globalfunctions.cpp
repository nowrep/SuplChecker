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
#include "globalfunctions.h"

QByteArray sc_pixmapToByteArray(const QPixmap &pix, const char *type)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    if (pix.save(&buffer, type))
        return buffer.buffer().toBase64();

    return QByteArray();
}

QByteArray sc_readAllFileContents(const QString &filename)
{
    QFile file(filename);
    file.open(QFile::ReadOnly);
    QByteArray a = file.readAll();
    file.close();

    return a;
}

void sc_centerWidgetOnScreen(QWidget *w)
{
    const QRect screen = QApplication::desktop()->screenGeometry();
    const QRect &size = w->geometry();
    w->move( (screen.width()-size.width())/2, (screen.height()-size.height())/2 );
}
