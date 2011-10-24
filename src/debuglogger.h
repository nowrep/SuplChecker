/*  SuplChecker - simple program to check a teacher's absencies at the school
    Copyright (C) 2010-2011  David Rosca

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
#ifndef DEBUGLOGGER_H
#define DEBUGLOGGER_H

#include <QWidget>

namespace Ui {
    class DebugLogger;
}

class DebugLogger : public QWidget
{
    Q_OBJECT

public:
    explicit DebugLogger(QWidget *parent = 0);
    ~DebugLogger();

    static void recordLog(const QString &log);

private slots:
    void clearLog();
    void loadLog();

private:
    static QString m_log;

    Ui::DebugLogger *ui;
};

#endif // DEBUGLOGGER_H
