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
#include <QPushButton>
#include <QtSql>
#include <QToolBar>
#include <QMenu>
#include <QToolButton>
#include <QWebFrame>
#include <QMenuBar>
#include <QFlags>
#include <QMenu>
#include <QTranslator>

namespace Ui {
    class SuplChecker;
}

class SuplChecker : public QMainWindow
{
    Q_OBJECT

public:
    void go();
    explicit SuplChecker(QWidget *parent = 0);
    ~SuplChecker();

    const QString DATADIR;

    void aktualizujUzivatele();

public slots:
    void zacni_loadovat(QString uzjmeno, QString uzheslo, QString server);

private:
    Ui::SuplChecker *ui;
    int spatneUdaje;
    int aktShown;
    QString aktJmeno;
    QToolButton* actUser;
    QMenu* actMenu;
    bool isLoading;

    enum StackedPages { CurrentWeek, NextWeek, Permanent, Marks };

private slots:
    void setLoading(bool set);
    void vycentruj();
    void info_o_programu();
    void chyba(QString text);
    void udaje();
    void aktualizace(QString stara, QString nova, QString changelog);
    void jmeno(QString jmeno, QString trida);
    void nacti(QString info, QByteArray data);
    void loadAction();
    void opakovat();
    void vybrano(QString text);
};
#endif // MAINWINDOW_H
