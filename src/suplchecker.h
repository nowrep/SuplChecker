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
#ifndef SUPLCHECKER_H
#define SUPLCHECKER_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFile>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QPushButton>
#include <QToolBar>
#include <QToolButton>
#include <QWebFrame>
#include <QMenuBar>
#include <QFlags>
#include <QMenu>
#include <QTranslator>
#include <QPointer>
#include <QFrame>
#include <QCloseEvent>
#include <QWebElementCollection>
#include <QShortcut>

#include "globalsettings.h"
#include "parser.h"

namespace Ui
{
class SuplChecker;
}

class AboutDialog;
class Parser;
class SuplChecker : public QMainWindow
{
    Q_OBJECT

public:
    explicit SuplChecker(QWidget* parent = 0);
    ~SuplChecker();

    static QIcon suplcheckerIcon();
    static const QString VERSION;
    static const QString BUILDTIME;
    static const QString AUTHOR;
    static const QString COPYRIGHT;
    static const QString WWWADDRESS;

public slots:
    void startLoading(GlobalSettings::User user);

private slots:
    void checkUpdates();
    void errorNoStartupUser();
    void aboutToShowUsersMenu();
    void aboutProgram();
    void chyba(Parser::Error er);
    void showSettingsDialog();
    void aktualizace(QString nova, QString changelog);
    void jmeno(const Parser::Student &s);
    void nacti(QString info, QByteArray data);
    void loadAction();
    void reloadWithSameUser();
    void vybrano(QString text);
    void deleteThread();

    void userModified(const GlobalSettings::User &before, const GlobalSettings::User &after);

private:
    void closeEvent(QCloseEvent* e);

    Ui::SuplChecker* ui;
    QToolButton* m_usersButton;
    QMenu* m_usersMenu;
    QPointer<AboutDialog> m_aboutDialog;
    Parser* m_threadParser;
    QPointer<QFrame> m_errorFrame;

    bool m_isLoading;

    GlobalSettings::User m_activeUser;
};
#endif // SUPLCHECKER_H
