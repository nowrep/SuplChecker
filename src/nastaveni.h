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

#ifndef NASTAVENI_H
#define NASTAVENI_H

#include <QDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTreeWidgetItem>
#include "globalsettings.h"

namespace Ui
{
class nastaveni;
}

class SuplChecker;
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(SuplChecker* mainWindow, QWidget* parent = 0);
    ~SettingsDialog();

signals:
    void userModified(GlobalSettings::User before, GlobalSettings::User after);

private slots:
    void showServerSettings();
    void addUser();
    void deleteUser();
    void userChanged(QTreeWidgetItem* item, int column);
    void itemSelectionChanged();

    void saveSettings();

private:
    Ui::nastaveni* ui;
    SuplChecker* m_mainWindow;
    GlobalSettings::User m_selectedUser;


};

#endif // NASTAVENI_H
