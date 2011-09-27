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
#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "debuglogger.h"
#include "suplchecker.h"
#include "globalfunctions.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AboutDialog)
    , m_debugLogger(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() ^ Qt::WindowMaximizeButtonHint ^ Qt::WindowMinimizeButtonHint);

    ui->setupUi(this);
    sc_centerWidgetOnScreen(this);

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(close()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(openLog()));
}

void AboutDialog::openLog()
{
    if (!m_debugLogger)
        m_debugLogger = new DebugLogger();

    m_debugLogger->show();
    m_debugLogger->raise();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
