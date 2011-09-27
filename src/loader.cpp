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
#include "loader.h"

Loader::Loader(QWidget* parent) :
    QLabel(parent)
{
    setFixedWidth(QPixmap(":icons/loader.gif").width());
}

void Loader::startAnimation()
{
    QMovie* movie = new QMovie(":icons/loader.gif");
    movie->setSpeed(70);
    setMovie(movie);
    movie->start();
}

void Loader::stopAnimation()
{
    movie()->stop();
    delete movie();
    clear();
}
