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

        bool operator==(const User &a)
        {
            return (a.name == name && a.password == password);
        }

        bool operator!=(const User &a)
        {
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

    static bool ShowDaysWithoutSubs;
    static bool CheckUpdates;

};

#endif // GLOBALSETTINGS_H
