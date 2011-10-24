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
#include <QtGui/QApplication>
#include <QTextCodec>
#include "suplchecker.h"
#include "debuglogger.h"

void messageOutput(QtMsgType type, const char *msg)
{
    QString originalMsg = QString::fromUtf8(msg);
    QString outputLine;

    switch (type) {
    case QtDebugMsg:
            outputLine = originalMsg;
        break;

    case QtWarningMsg:
        outputLine = "WARNING: " + originalMsg;
        break;

    case QtFatalMsg:
        outputLine = "FATAL: " + originalMsg;
        abort();
        break;

    case QtCriticalMsg:
        outputLine = "CRITICAL: " + originalMsg;
        break;
    }

    if (outputLine.isEmpty())
        return;

    outputLine.prepend("[" + QTime::currentTime().toString("hh:mm:ss:zzz") +"] ");
    DebugLogger::recordLog(outputLine);
}

int main(int argc, char *argv[])
{
    qInstallMsgHandler(messageOutput);

    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    SuplChecker w;
    w.show();

    return a.exec();
}
