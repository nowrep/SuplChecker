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
#include "globalsettings.h"
#include "globalfunctions.h"
#include <QApplication>

QString GlobalSettings::DataDir;
GlobalSettings::User GlobalSettings::StartupUser;
QList<GlobalSettings::User> GlobalSettings::AllUsers;
QStringList GlobalSettings::AvailableServers;
QString GlobalSettings::BackgroundPixmapPath;
QString GlobalSettings::BackgroundPixmapName;
QByteArray GlobalSettings::m_bgPixmapData;

bool GlobalSettings::m_reloadPixmap = true;
bool GlobalSettings::ShowDaysWithoutSubs;
bool GlobalSettings::CheckUpdates;

GlobalSettings::GlobalSettings()
{
}

void GlobalSettings::setBackgroundPixmap(const QString &name)
{
    m_reloadPixmap = true;
    BackgroundPixmapName = name;
    BackgroundPixmapPath = ":html/" + name;
}

QByteArray GlobalSettings::backgroundPixmapData()
{
    if (m_reloadPixmap) {
        m_bgPixmapData = sc_pixmapToByteArray(QPixmap(BackgroundPixmapPath));
        m_reloadPixmap = false;
    }

    return m_bgPixmapData;
}

void GlobalSettings::loadSettings()
{
    DataDir = qApp->applicationDirPath() + "/data/";

    // Ensuring settings file is writeable
    QFile settingsFile(DataDir + "/settings.ini");
    if (settingsFile.exists()) {
        settingsFile.setPermissions(QFile::ReadUser | QFile::WriteUser);
    }

    QSettings settings(DataDir + "/settings.ini", QSettings::IniFormat);
    settings.beginGroup("Obecne");
    AvailableServers = settings.value("servers", QStringList() << "http://g8mb.cz/bakaweb/" << "http://gserver/bakaweb/").toStringList();
    ShowDaysWithoutSubs = settings.value("zobrazitDnyBezSuplovani", true).toBool();
    CheckUpdates = settings.value("sledovatAktualizace", true).toBool();
    setBackgroundPixmap(settings.value("pozadi", "bg-blue.png").toString());
    settings.endGroup();

    settings.beginGroup("Uzivatele");
    QStringList names = settings.value("uzivatele", QStringList("jmeno")).toStringList();
    QStringList passwords = settings.value("hesla", QStringList("heslo")).toStringList();
    QStringList realNames = settings.value("celaJmena", QStringList("")).toStringList();

    QString actUser = settings.value("defaultniUzivatel", "").toString();
    settings.endGroup();

    for (int i = 0; i < names.count(); i++) {
        if (passwords.count() <= i) {
            continue;
        }
        if (realNames.count() <= i) {
            continue;
        }

        User user;
        user.name = names.at(i);
        user.password = passwords.at(i);
        user.realName = realNames.at(i);
        AllUsers.append(user);
    }

    if (actUser != "") {
        foreach(User usr, AllUsers) {
            if (usr.name == actUser) {
                StartupUser = usr;
                return;
            }
        }
    }

    StartupUser = AllUsers.first();
}

void GlobalSettings::saveSettings()
{
    QSettings settings(DataDir + "/settings.ini", QSettings::IniFormat);
    settings.beginGroup("Obecne");
    settings.setValue("servers", AvailableServers);
    settings.setValue("zobrazitDnyBezSuplovani", ShowDaysWithoutSubs);
    settings.setValue("sledovatAktualizace", CheckUpdates);
    settings.setValue("pozadi", BackgroundPixmapName);

    settings.endGroup();

    QStringList names;
    QStringList passwords;
    QStringList realNames;
    foreach(User usr, AllUsers) {
        names.append(usr.name);
        passwords.append(usr.password);
        realNames.append(usr.realName);
    }

    settings.beginGroup("Uzivatele");
    settings.setValue("defaultniUzivatel", StartupUser.name);
    settings.setValue("uzivatele", names);
    settings.setValue("hesla", passwords);
    settings.setValue("celaJmena", realNames);
    settings.endGroup();
}
