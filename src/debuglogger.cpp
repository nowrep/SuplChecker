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
#include "debuglogger.h"
#include "ui_debuglogger.h"
#include "suplchecker.h"
#include "globalfunctions.h"

QString DebugLogger::m_log;

DebugLogger::DebugLogger(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DebugLogger)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() ^ Qt::WindowMaximizeButtonHint ^ Qt::WindowMinimizeButtonHint);

    ui->setupUi(this);
    sc_centerWidgetOnScreen(this);

    loadLog();

    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearLog()));
    connect(ui->obnovit, SIGNAL(clicked()), this, SLOT(loadLog()));

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(close()));
}

void DebugLogger::loadLog()
{
    ui->textBrowser->setText(m_log);
}

void DebugLogger::clearLog()
{
    m_log.clear();
    ui->textBrowser->clear();
}

void DebugLogger::recordLog(const QString &log)
{
    if (log.isEmpty())
        return;

    m_log.prepend(log + "\n");
}

DebugLogger::~DebugLogger()
{
    delete ui;
}
