/* ============================================================
* SuplChecker - simple program to check a teacher's absencies at school
* Copyright (C) 2010-2012  David Rosca <david@rosca.cz>
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
* ============================================================ */
#include "serversdialog.h"
#include "ui_serversdialog.h"
#include "globalsettings.h"

ServersDialog::ServersDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ServersDialog)
{
    ui->setupUi(this);

    foreach(QString sr, GlobalSettings::AvailableServers) {
        QListWidgetItem* i = new QListWidgetItem();
        i->setText(sr);
        i->setFlags(i->flags() | Qt::ItemIsEditable);
        ui->listWidget->addItem(i);
    }

    ui->listWidget->setCurrentRow(0);

    connect(ui->nahoru, SIGNAL(clicked()), this, SLOT(itemUp()));
    connect(ui->dolu, SIGNAL(clicked()), this, SLOT(itemDown()));
    connect(ui->smazat, SIGNAL(clicked()), this, SLOT(itemDelete()));
    connect(ui->novy, SIGNAL(clicked()), this, SLOT(itemNew()));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
}

void ServersDialog::itemUp()
{
    int index = ui->listWidget->currentRow();
    QListWidgetItem* currentItem = ui->listWidget->currentItem();

    if (!currentItem || index == 0) {
        return;
    }

    ui->listWidget->takeItem(index);
    ui->listWidget->insertItem(index - 1, currentItem);
    ui->listWidget->setCurrentItem(currentItem);
}

void ServersDialog::itemDown()
{
    int index = ui->listWidget->currentRow();
    QListWidgetItem* currentItem = ui->listWidget->currentItem();

    if (!currentItem || index == ui->listWidget->count() - 1) {
        return;
    }

    ui->listWidget->takeItem(index);
    ui->listWidget->insertItem(index + 1, currentItem);
    ui->listWidget->setCurrentItem(currentItem);
}

void ServersDialog::itemDelete()
{
    QListWidgetItem* currentItem = ui->listWidget->currentItem();

    if (!currentItem || ui->listWidget->count() == 1) {
        return;
    }

    delete currentItem;
}

void ServersDialog::itemNew()
{
    QListWidgetItem* i = new QListWidgetItem();
    i->setText("http://");
    i->setFlags(i->flags() | Qt::ItemIsEditable);
    ui->listWidget->addItem(i);

    ui->listWidget->setCurrentItem(i);
}

void ServersDialog::saveSettings()
{
    QStringList allServers;

    for (int i = 0; i < ui->listWidget->count(); i++) {
        QListWidgetItem* item = ui->listWidget->item(i);
        if (!item) {
            continue;
        }

        QString text = item->text();
        if (text.isEmpty()) {
            continue;
        }

        allServers.append(text);
    }

    GlobalSettings::AvailableServers = allServers;

    close();
}

ServersDialog::~ServersDialog()
{
    delete ui;
}
