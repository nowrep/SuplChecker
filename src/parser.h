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
#ifndef PARSER_H
#define PARSER_H

#include <QThread>
#include <QtNetwork/QtNetwork>

class Parser : public QThread
{
    Q_OBJECT
public:
    Parser(QString jmeno, QString heslo, bool checkUpdates);
    ~Parser();

    void setBackgroundData(const QByteArray &data) { m_bgData = data; }

    enum Error { NoServersAvailable, BadLogins };
    enum Operation { GET, POST };
    struct Student {
        QString jmeno;
        QString trida;
    };

protected:
   void run();

signals:
   void studentName(Parser::Student s);
   void aktualizace(const QString &nova, const QString &stara, const QString &changelog);
   void error(Parser::Error er);
   void done(const QString &results, const QByteArray &data);
   void loading(bool state);
   void deleteNow();

private:
   QByteArray pripravHtml(bool includeBasicStyl = false);

   QString vrat_den(int den, const QString &zdroj);
   QString vrat_tyden(const QString &zdroj);
   Student vrat_jmeno(const QString &zdroj);
   bool zjisti_supl(const QString &zdroj);
   QString checkni_tyden(const QString &zdroj);

   void parsuj_dalsi(const QString &zdroj, const QString &soubor);
   void parsuj_tyden(const QString &zdroj, const QString &soubor);

   QString send_request(const QUrl &url, Operation method = GET, QByteArray postData = "");
   QString getInput(const QString &zdroj, const QString &input);
   bool isServerOnline(const QString &server);

   QNetworkAccessManager* m_manager;
   QString m_userName;
   QString m_userPassword;
   QString m_activeServer;
   QByteArray m_bgData;
   bool m_checkUpdates;

   QString _send_request(QNetworkAccessManager *manager, QString url, int method = 1, QByteArray postData = "");

private slots:
   void work();
   void startWork();

};

#endif // PARSER_H
