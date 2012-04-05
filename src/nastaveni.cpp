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
#include "nastaveni.h"
#include "ui_nastaveni.h"
#include "suplchecker.h"
#include "globalsettings.h"
#include "serversdialog.h"

QString colorToFile(const QString &color)
{
    if (color == "Modrá") {
        return "bg-blue.png";
    }
    if (color == "Zelená") {
        return "bg-green.png";
    }
    if (color == "Fialová") {
        return "bg-purple.png";
    }
    if (color == "Červená") {
        return "bg-red.png";
    }

    return "bg-blue.png";
}

QString fileToColor(const QString &file)
{
    if (file == "bg-blue.png") {
        return "Modrá";
    }
    if (file == "bg-green.png") {
        return "Zelená";
    }
    if (file == "bg-purple.png") {
        return "Fialová";
    }
    if (file == "bg-red.png") {
        return "Červená";
    }

    return "Modrá";
}

SettingsDialog::SettingsDialog(SuplChecker* mainWindow, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::nastaveni)
    , m_mainWindow(mainWindow)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
#ifdef Q_WS_WIN
    resize(600, 400);
#endif
    ui->zobrazDnyBezSuplovani->setChecked(GlobalSettings::ShowDaysWithoutSubs);
    ui->kontrolovatAktualizace->setChecked(GlobalSettings::CheckUpdates);

    ui->startUzivatel->addItem(GlobalSettings::StartupUser.name);
    foreach(GlobalSettings::User usr, GlobalSettings::AllUsers) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->uzivatele);
        item->setText(0, usr.name);
        item->setText(1, usr.password);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->uzivatele->addTopLevelItem(item);

        if (usr == GlobalSettings::StartupUser) {
            continue;
        }
        ui->startUzivatel->addItem(usr.name);
    }

    for (int i = 0; i < ui->barvaPozadi->count(); i++) {
        if (colorToFile(ui->barvaPozadi->itemText(i)) == GlobalSettings::BackgroundPixmapName) {
            ui->barvaPozadi->setCurrentIndex(i);
            break;
        }
    }

    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(ui->servery, SIGNAL(clicked()), this, SLOT(showServerSettings()));

    connect(ui->uzivatele, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(userChanged(QTreeWidgetItem*, int)));
    connect(ui->uzivatele, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
    connect(ui->butNovy, SIGNAL(clicked()), this, SLOT(addUser()));
    connect(ui->butSmazat, SIGNAL(clicked()), this, SLOT(deleteUser()));

    ui->uzivatele->setFocus();
    ui->uzivatele->itemAt(0, 0)->setSelected(true);

    QTimer::singleShot(0, this, SLOT(itemSelectionChanged()));
}

void SettingsDialog::addUser()
{
    foreach(GlobalSettings::User usr, GlobalSettings::AllUsers) {
        if (usr.name == "Nový") {
            return;
        }
    }

    QTreeWidgetItem* item = new QTreeWidgetItem(ui->uzivatele);
    item->setText(0, "Nový");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->uzivatele->addTopLevelItem(item);

    GlobalSettings::User user;
    user.name = "Nový";
    user.password = "";
    user.realName = "";
    GlobalSettings::AllUsers.append(user);
    ui->startUzivatel->addItem("Nový");
}

void SettingsDialog::deleteUser()
{
    if (!ui->uzivatele->currentItem() || ui->uzivatele->topLevelItemCount() == 1) {
        return;
    }

    GlobalSettings::User usr;
    usr.name = ui->uzivatele->currentItem()->text(0);
    usr.password = ui->uzivatele->currentItem()->text(1);

    if (GlobalSettings::AllUsers.contains(usr)) {
        GlobalSettings::AllUsers.removeOne(usr);
        delete ui->uzivatele->currentItem();
        ui->startUzivatel->removeItem(ui->startUzivatel->findText(usr.name));
    }
}

void SettingsDialog::itemSelectionChanged()
{
    if (!ui->uzivatele->currentItem()) {
        return;
    }

    GlobalSettings::User usr;
    usr.name = ui->uzivatele->currentItem()->text(0);
    usr.password = ui->uzivatele->currentItem()->text(1);
    usr.realName = "";
    m_selectedUser = usr;
}

void SettingsDialog::userChanged(QTreeWidgetItem* item, int column)
{
    if (GlobalSettings::AllUsers.contains(m_selectedUser)) {
        if (column == 0 && ui->uzivatele->findItems(item->text(0), Qt::MatchExactly).count() > 1) {
            item->setText(0, m_selectedUser.name);
        }
        GlobalSettings::User usr;
        usr.name = ui->uzivatele->currentItem()->text(0);
        usr.password = ui->uzivatele->currentItem()->text(1);
        usr.realName = "";

        GlobalSettings::AllUsers.removeOne(m_selectedUser);
        GlobalSettings::AllUsers.append(usr);

        if (column == 0) {
            int editedIndex = ui->startUzivatel->findText(m_selectedUser.name);
            if (editedIndex != -1) {
                ui->startUzivatel->setItemText(editedIndex, usr.name);
            }
        }

        emit userModified(m_selectedUser, usr);
        m_selectedUser = usr;
    }
}

void SettingsDialog::showServerSettings()
{
    ServersDialog d(this);
    d.exec();
}

void SettingsDialog::saveSettings()
{
    GlobalSettings::ShowDaysWithoutSubs = ui->zobrazDnyBezSuplovani->isChecked();
    GlobalSettings::CheckUpdates = ui->kontrolovatAktualizace->isChecked();
    GlobalSettings::setBackgroundPixmap(colorToFile(ui->barvaPozadi->currentText()));

//    GlobalSettings::AvailableServers = ui->server->currentText();

    foreach(GlobalSettings::User usr, GlobalSettings::AllUsers) {
        if (usr.name == ui->startUzivatel->currentText()) {
            GlobalSettings::StartupUser = usr;
            break;
        }
    }

    close();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
