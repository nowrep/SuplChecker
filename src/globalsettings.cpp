#include "globalsettings.h"
#include <QApplication>

QString GlobalSettings::DataDir;
GlobalSettings::User GlobalSettings::StartupUser;
QList<GlobalSettings::User> GlobalSettings::AllUsers;
QStringList GlobalSettings::AvailableServers;
QString GlobalSettings::BackgroundPixmapPath;
QString GlobalSettings::BackgroundPixmapName;

bool GlobalSettings::ShowDaysWithoutSubs;
bool GlobalSettings::CheckUpdates;

GlobalSettings::GlobalSettings()
{
}

void GlobalSettings::loadSettings()
{
    DataDir = qApp->applicationDirPath() + "/data/";

    QSettings settings(DataDir + "/settings.ini", QSettings::IniFormat);
    settings.beginGroup("Obecne");
    AvailableServers = settings.value("servers", QStringList() << "http://g8mb.cz/bakaweb/" << "http://gserver/bakaweb/").toStringList();
    ShowDaysWithoutSubs = settings.value("zobrazitDnyBezSuplovani", true).toBool();
    CheckUpdates = settings.value("sledovatAktualizace", true).toBool();
    BackgroundPixmapName = settings.value("pozadi", "bg-blue.png").toString();
    BackgroundPixmapPath = ":html/" + BackgroundPixmapName;
    settings.endGroup();

    settings.beginGroup("Uzivatele");
    QStringList names = settings.value("uzivatele", QStringList("jmeno")).toStringList();
    QStringList passwords = settings.value("hesla", QStringList("heslo")).toStringList();
    QStringList realNames = settings.value("celaJmena", QStringList("")).toStringList();

    QString actUser = settings.value("defaultniUzivatel", "").toString();
    settings.endGroup();

    for (int i = 0; i < names.count(); i++) {
        if (passwords.count() <= i)
            continue;
        if (realNames.count() <= i)
            continue;

        User user;
        user.name = names.at(i);
        user.password = passwords.at(i);
        user.realName = realNames.at(i);
        AllUsers.append(user);
    }

    if (actUser != "") {
        foreach (User usr, AllUsers) {
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
    foreach (User usr, AllUsers) {
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
