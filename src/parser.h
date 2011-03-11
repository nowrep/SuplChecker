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
#ifndef NEWTHREAD_H
#define NEWTHREAD_H

#include <QThread>
#include <QtNetwork/QtNetwork>

class Parser : public QThread
{
    Q_OBJECT
public:
    Parser(QString jmeno, QString heslo, QString server);
protected:
   void run();

signals:
   void jmeno(const QString &jmeno, const QString &trida);
   void aktualizace(const QString &nova, const QString &stara, const QString &changelog);
   void chyba(const QString &text);
   void zobraz_udaje();
   void done(const QString &results, const QByteArray &data);
   void loading(bool state);

private:
   QString uzJmeno;
   QString uzHeslo;
   QString uzServer;
private slots:
    QString vrat_den(int den, QString zdroj);
    QString vrat_tyden(QString zdroj);
    void vrat_jmeno(QString zdroj);
    bool zjisti_supl(QString zdroj);
    QString checkni_tyden(QString zdroj);
    QString send_request(QNetworkAccessManager *manager, QString url, int method=1, QByteArray postData="");
    void parsuj_dalsi(QString zdroj,QString soubor);
    void parsuj_tyden(QString zdroj,QString soubor);
    QString getInput(QString zdroj, QString input);
    void pracuj();
};

#endif // NEWTHREAD_H
