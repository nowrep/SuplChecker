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
#ifndef SERVERSDIALOG_H
#define SERVERSDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui
{
class ServersDialog;
}

class ServersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServersDialog(QWidget* parent = 0);
    ~ServersDialog();

private slots:
    void itemUp();
    void itemDown();
    void itemDelete();
    void itemNew();

    void saveSettings();

private:
    Ui::ServersDialog* ui;
};

#endif // SERVERSDIALOG_H
