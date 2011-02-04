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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFile>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QTextCodec>
#include <QPushButton>
#include <QtSql>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void go();
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void zacni_loadovat(QString uzjmeno, QString uzheslo, QString server);

private:
    Ui::MainWindow *ui;
    int spatneUdaje;
    int aktShown;
    QString aktJmeno;

private slots:
    void vycentruj();
    void info_o_programu();
    void chyba(QString text);
    void udaje();
    void aktualizace(QString stara, QString nova, QString changelog);
    void jmeno(QString jmeno, QString trida);
    void nacti(QString info);
    void opakovat();
    void zobraz_vyber();
    void vybrano(QString text);
};
#endif // MAINWINDOW_H
