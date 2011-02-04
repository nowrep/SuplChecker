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

#ifndef NASTAVENI_H
#define NASTAVENI_H

#include <QDialog>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <QTreeWidgetItem>
#include "mainwindow.h"

namespace Ui {
    class nastaveni;
}

class nastaveni : public QDialog
{
    Q_OBJECT

public:
    explicit nastaveni(MainWindow* mainWindow,QWidget *parent = 0);
    ~nastaveni();

private:
    Ui::nastaveni *ui;
    MainWindow* mainwindow;
    QString aktJmeno;

private slots:
    void zapis_udaje();
    void aktualizuj_tabulku();
    void pridat_uziv();
    void zobraz_kliknuty(QTreeWidgetItem *item, int column);
    void smazat_uziv();
    void nacti();
};

#endif // NASTAVENI_H
