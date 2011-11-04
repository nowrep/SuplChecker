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
#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "debuglogger.h"
#include "suplchecker.h"
#include "globalfunctions.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
    , m_debugLogger(0)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    ui->textBrowser->setHtml(QString("<center><h2>SuplChecker</h2>"
                             "<b>Verze:</b> %1 <br /><b>Datum:</b> %2 <br /><br />"
                             "<b>Autor:</b><br/> %3<br /><b>Poděkování:</b><br/> Rajnymu a Patrickovi<br /><br />"
                             "<small>Copyright (C) %4 %5</small><br />"
                             "<a href='%6'>%6</center>"
                             ).arg(SuplChecker::VERSION, SuplChecker::BUILDTIME, SuplChecker::AUTHOR, SuplChecker::COPYRIGHT, SuplChecker::AUTHOR, SuplChecker::WWWADDRESS)
                             );

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(close()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(openLog()));
}

void AboutDialog::openLog()
{
    close();

    if (!m_debugLogger)
        m_debugLogger = new DebugLogger(parentWidget());

    m_debugLogger->show();
    m_debugLogger->raise();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
