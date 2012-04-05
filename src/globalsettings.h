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
#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H
#include <QString>
#include <QSettings>
#include <QStringList>
#include <QPixmap>

class GlobalSettings
{
public:
    struct User {
        QString name;
        QString password;
        QString realName;

        bool operator==(const User &a) {
            return (a.name == this->name && a.password == this->password);
        }

        bool operator!=(const User &a) {
            return !(*this == a);
        }
    };

    GlobalSettings();
    static void loadSettings();
    static void saveSettings();

    static QString DataDir;

    static User StartupUser;
    static QList<User> AllUsers;
    static QStringList AvailableServers;

    static QString BackgroundPixmapPath;
    static QString BackgroundPixmapName;
    static void setBackgroundPixmap(const QString &name);

    static bool ShowDaysWithoutSubs;
    static bool CheckUpdates;

    static QByteArray backgroundPixmapData();

private:
    static bool m_reloadPixmap;
    static QByteArray m_bgPixmapData;
};

#endif // GLOBALSETTINGS_H
