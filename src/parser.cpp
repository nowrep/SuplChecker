/*  SuplChecker - simple program to check a teacher's absencies at the school
    Copyright (C) 2010-2011  David Rosca

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
#include "parser.h"
#include "globalsettings.h"
#include "globalfunctions.h"
#include "suplchecker.h"

Parser::Parser(const QString &jmeno, const QString &heslo)
    : QThread()
    , m_manager(0)
    , m_userName(jmeno)
    , m_userPassword(heslo)
{
    qRegisterMetaType<Error>("Parser::Error");
    qRegisterMetaType<Student>("Parser::Student");
}

QString Parser::vrat_den(int den, const QString &zdroj)
{
    QString vyraz;
    switch (den) {
        case 1: vyraz = "<tr><td class=\"rozden\" Rowspan=3>Po(.*)<tr><td class=\"rozden\" Rowspan=3>"; break;
        case 2: vyraz = "<tr><td class=\"rozden\" Rowspan=3>Út(.*)<tr><td class=\"rozden\" Rowspan=3>"; break;
        case 3: vyraz = "<tr><td class=\"rozden\" Rowspan=3>St(.*)<tr><td class=\"rozden\" Rowspan=3>"; break;
        case 4: vyraz = "<tr><td class=\"rozden\" Rowspan=3>Čt(.*)<tr><td class=\"rozden\" Rowspan=3>"; break;
        case 5: vyraz = "<tr><td class=\"rozden\" Rowspan=3>Pá(.*)</tbody></table>"; break;
    }

    QRegExp rx(vyraz, Qt::CaseInsensitive);
    rx.setMinimal(true);
    rx.indexIn(zdroj);
    QString captured = rx.cap(1);
    switch (den) {
        case 1: captured = "<tr><td class=\"rozden\" Rowspan=3>Po" + captured; break;
        case 2: captured = "<tr><td class=\"rozden\" Rowspan=3>Út" + captured; break;
        case 3: captured = "<tr><td class=\"rozden\" Rowspan=3>St" + captured; break;
        case 4: captured = "<tr><td class=\"rozden\" Rowspan=3>Čt" + captured; break;
        case 5: captured = "<tr><td class=\"rozden\" Rowspan=3>Pá" + captured; break;
    }

    if (rx.indexIn(zdroj))
        return captured;

    return ""; // den nebyl nalezen -> bug?
}

QString Parser::vrat_tyden(const QString &zdroj)
{
    QString vyraz = "<div class=\"rozpopishod\">(.*)</div>";
    QRegExp rx(vyraz, Qt::CaseInsensitive);
    rx.setMinimal(true);
    rx.indexIn(zdroj);
    QString od = rx.cap(1);
    rx.lastIndexIn(zdroj);
    QString dod = rx.cap(1);
    return "<center><h1> " + od + " - " + dod + " " + QDateTime::currentDateTime().toString("yyyy") + "</center></h1>";
}

bool Parser::zjisti_supl(const QString &zdroj)
{
    QString vyraz = "<td class=\"rozpredmet1zm\" ";
    QString vyraz2 = "<td class=\"rozpredmet1abs\" ";
    return (zdroj.contains(vyraz) || zdroj.contains(vyraz2));
}

QString Parser::checkni_tyden(const QString &zdroj)
{
    QString vystup = "";

    QString pondeli = vrat_den(1, zdroj);
    QString utery = vrat_den(2, zdroj);
    QString streda = vrat_den(3, zdroj);
    QString ctvrtek = vrat_den(4, zdroj);
    QString patek = vrat_den(5, zdroj);

    if (GlobalSettings::ShowDaysWithoutSubs) {
        vystup += pondeli + "\n";
        vystup += utery + "\n";
        vystup += streda + "\n";
        vystup += ctvrtek + "\n";
        vystup += patek + "\n";

        return vystup;
    }

    bool po_supl = zjisti_supl(pondeli);
    bool ut_supl = zjisti_supl(utery);
    bool st_supl = zjisti_supl(streda);
    bool ct_supl = zjisti_supl(ctvrtek);
    bool pa_supl = zjisti_supl(patek);

    if (po_supl)
        vystup += pondeli + "\n";

    if (ut_supl)
        vystup += utery + "\n";

    if (st_supl)
        vystup += streda + "\n";

    if (ct_supl)
        vystup += ctvrtek + "\n";

    if (pa_supl)
        vystup += patek + "\n";


    return vystup;
}

QByteArray Parser::pripravHtml(bool includeExtraStyl)
{
    QByteArray html = sc_readAllFileContents(":html/result.html");
    QByteArray style = sc_readAllFileContents(":html/styl.css");
    if (includeExtraStyl)
        style.append(sc_readAllFileContents(":html/extrastyl.css"));

    style.replace("%BG-IMG-NAME%", GlobalSettings::BackgroundPixmapName.toAscii());
    html.replace("%CSS-STYLE%", style);

    return html;
}

void Parser::parsuj_tyden(const QString &zdroj, const QString &soubor)
{
    QString vyraz = "<table cellspacing=\"0\" cellpadding=\"0\" class=\"rozbunka\" >(.*)</td></tr>(.*)</td></tr>";
    QRegExp rx(vyraz, Qt::CaseInsensitive);
    rx.setMinimal(true);
    rx.indexIn(zdroj);
    QString zac_tabulky = rx.cap(0).replace("<br />", "-");

    QString temp = checkni_tyden(zdroj);
    if (temp == "") {
        temp = "<center>V požadovanou dobu nenalezeno žádné suplování!</center>";
        zac_tabulky = "";
    }
    QString datum = vrat_tyden(zdroj);

    QByteArray body;
    body.append(zac_tabulky + datum + temp + "</tbody></table>");
    QByteArray data = pripravHtml();
    data.replace("%BODY%", body);

    emit done(soubor, data);
}

void Parser::parsuj_dalsi(const QString &zdroj, const QString &soubor)
{
    QByteArray data;
    QByteArray body;

    if (soubor == "stalyrozvrh.html") {
        QString vyraz = "<table cellspacing=\"0\" cellpadding=\"0\" class=\"rozbunka\" >(.*)\n";
        QRegExp rx(vyraz, Qt::CaseInsensitive);
        rx.setMinimal(true);
        rx.indexIn(zdroj);
        QString captured = rx.cap(0);

        data = pripravHtml();
        body.append("<center><h1>Stálý rozvrh</h1></center>");
        body.append(captured);
        body.append("</tbody></table>");
    }
    else if (soubor == "znamky.html") {
        QString vyraz = "<table class=\"radekznamky\">(.*)</tbody></table>";
        QRegExp rx(vyraz, Qt::CaseInsensitive);
        rx.setMinimal(true);
        rx.indexIn(zdroj);
        QString captured = rx.cap(0);

        data = pripravHtml(true);
        body.append("<center><h1>Průběžná klasifikace</h1>");
        body.append(captured);
        body.append("</tbody></table>");
    }
    else if (soubor=="pololetni.html") {
        QString vyraz = "<div class=\"modulin\" >(.*)</tbody></table>";
        QRegExp rx(vyraz, Qt::CaseInsensitive );
        rx.setMinimal(true);
        rx.indexIn(zdroj);
        QString captured = rx.cap(0);

        QRegExp rx2("<tr><td class=\"polonazev\">Chování(.*)</tr>", Qt::CaseInsensitive);
        rx2.setMinimal(true);
        captured.remove(rx2);

        captured.replace("<table>", "<table class=\"suplchecker_table\">");
        data = pripravHtml(true);
        body.append("<center>" + captured + "</center>");
    }
    else if (soubor == "planakci.html") {
        QString vyraz = "<table class=\"pltable\"(.*)</tbody></table>";
        QRegExp rx(vyraz, Qt::CaseInsensitive );
        rx.setMinimal(true);
        rx.indexIn(zdroj);
        QString captured = rx.cap(0);

        data = pripravHtml(true);
        body.append("<center>" + captured + "</center>");
    }
    else {
        qWarning("Parser::parsuj_dalsi neznamy soubor");
        return;
    }

    data.replace("%BODY%", body);

    emit done(soubor, data);
}

Parser::Student Parser::vrat_jmeno(const QString &zdroj)
{
    QString vyraz = "<td style=\"width:100%;\" class=\"logjmeno\" colspan=\"2\">(.*), (.*)</td>";
    QRegExp rx(vyraz, Qt::CaseInsensitive);
    rx.setMinimal(true);
    rx.indexIn(zdroj);

    Student s;
    s.jmeno = rx.cap(1);
    s.trida = rx.cap(2);
    return s;
}

QString Parser::getInput(const QString &zdroj, const QString &input)
{
    QString vyraz = "<input type=\"hidden\" name=\"__" + input + "\" id=\"__" + input + "\" value=\"(.*)\"";
    QRegExp rx(vyraz, Qt::CaseInsensitive);
    rx.setMinimal(true);
    rx.indexIn(zdroj);
    QString vysl = rx.cap(1);
    QByteArray temp;
    temp.append(vysl);
    QString vysledek = temp.toPercentEncoding();
    return vysledek;
}

QString Parser::send_request(const QUrl &url, Operation method, QByteArray postData)
{
    Q_ASSERT(m_manager);

    QNetworkReply* reply;
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "SuplChecker by davros");

    if (method == GET)
        reply = m_manager->get(req);
    else if (method == POST) {
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        reply = m_manager->post(req, postData);
    }

    QEventLoop eLoop;
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), &eLoop, SLOT(quit()));
    eLoop.exec();

    return reply->readAll();
}

bool Parser::isServerOnline(const QString &server)
{
    QString reply = send_request(server);
    return !reply.isEmpty();
}

void Parser::run()
{
    QTimer::singleShot(0, this, SLOT(startWork()));
    exec();
}

void Parser::startWork()
{
    qDebug() << "starting loading thread id: " << currentThreadId();

    emit loading(true);
    work();

    exit();
    emit loading(false);
    emit deleteNow();

    qDebug() << "finishing thread id: " << currentThreadId();
}

void Parser::work()
{
    m_manager = new QNetworkAccessManager(this);

    foreach (QString sr, GlobalSettings::AvailableServers) {
        bool isOnline = isServerOnline(sr);
        if (isOnline) {
            qDebug() << "server " << sr << " is online";
            m_activeServer = sr;
            break;
        }
        else
            qDebug() << "server" << sr << " is offline";
    }

    if (m_activeServer.isEmpty()) {
        qWarning() << "all servers offline. check your connection?";
        emit error(NoServersAvailable);
        return;
    }

    QByteArray postData;
    postData.append("ctl00$cphmain$TextBoxjmeno=" + m_userName + "&");
    postData.append("ctl00$cphmain$TextBoxheslo=" + m_userPassword + "&");
    postData.append("__VIEWSTATE=&");
    postData.append("ctl00$cphmain$ButtonPrihlas=P%C5%99ihl%C3%A1sit&");
    send_request(m_activeServer + "login.aspx", POST, postData);

    QString tento_tyden = send_request(m_activeServer + "prehled.aspx?s=6"); // tento tyden je doma
    Student s = vrat_jmeno(tento_tyden);
    if (s.jmeno.isEmpty() || s.trida.isEmpty()) {
        qWarning() << "bad logins!";
        emit error(BadLogins);
        return;
    }
    emit studentName(s);

    parsuj_tyden(tento_tyden, "tentotyden.html"); // :-)

    ///PRISTI TYDEN
    QString viewstate = getInput(tento_tyden, "VIEWSTATE");
    QString eventtarget = getInput(tento_tyden, "EVENTTARGET");
    QString eventvalidation = getInput(tento_tyden, "EVENTVALIDATION");

    postData.clear();
    postData.append("__EVENTTARGET=" + eventtarget + "&");
    postData.append("__EVENTVALIDATION=" + eventvalidation + "&");
    postData.append("__VIEWSTATE=" + viewstate + "&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24checkucitel=on&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24checkskupina=on&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24Checkmistnost=on&");
    postData.append("ctl00%24cphmain%24radiorozvrh=rozvrh+na+p%C5%99%C3%AD%C5%A1t%C3%AD+t%C3%BDden");
    QString pristi_tyden = send_request(m_activeServer + "prehled.aspx?s=6", POST, postData); // pristi tyden je doma
    parsuj_tyden(pristi_tyden, "dalsityden.html");

    ///STALY ROZVRH
    viewstate = getInput(pristi_tyden, "VIEWSTATE");
    eventtarget = getInput(pristi_tyden, "EVENTTARGET");
    eventvalidation = getInput(pristi_tyden, "EVENTVALIDATION");

    postData.clear();
    postData.append("__EVENTTARGET=" + eventtarget + "&");
    postData.append("__EVENTVALIDATION=" + eventvalidation + "&");
    postData.append("__VIEWSTATE=" + viewstate + "&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24checkucitel=on&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24checkskupina=on&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24Checkmistnost=on&");
    postData.append("ctl00%24cphmain%24radiorozvrh=st%C3%A1l%C3%BD+rozvrh");
    QString staly_rozvrh = send_request(m_activeServer + "prehled.aspx?s=6", POST, postData); // staly rozvrh je doma
    parsuj_dalsi(staly_rozvrh, "stalyrozvrh.html");

    ///ZNAMKY
    QString temp = send_request(m_activeServer + "prehled.aspx?s=2"); //
    viewstate = getInput(temp, "VIEWSTATE");
    eventtarget = getInput(temp, "EVENTTARGET");
    eventvalidation = getInput(temp, "EVENTVALIDATION");

    postData.clear();
    postData.append("__VIEWSTATE=" + viewstate + "&");
    postData.append("__EVENTTARGET=" + eventtarget + "&");
    postData.append("__EVENTVALIDATION=" + eventvalidation + "&");
    postData.append("__EVENTARGUMENT=&");
    postData.append("__LASTFOCUS=&");
    postData.append("ctl00%24cphmain%24Flyout2%24Checkprumery=on&");
    postData.append("ctl00%24cphmain%24listdoba=pololeti&");
    QString znamky = send_request(m_activeServer + "prehled.aspx?s=2", POST, postData); // znamky jsou doma
    parsuj_dalsi(znamky, "znamky.html");

    //POLOLETNI ZNAMKY
    viewstate = getInput(temp, "VIEWSTATE");
    eventtarget = getInput(temp, "EVENTTARGET");
    eventvalidation = getInput(temp, "EVENTVALIDATION");

    postData.clear();
    postData.append("__VIEWSTATE=" + viewstate + "&");
    postData.append("__EVENTTARGET=" + eventtarget + "&");
    postData.append("__EVENTVALIDATION=" + eventvalidation + "&");
    postData.append("__EVENTARGUMENT=&");
    postData.append("__LASTFOCUS=&");
    QString pololetni = send_request(m_activeServer + "prehled.aspx?s=4", POST, postData); // pololetni znamky jsou doma
    parsuj_dalsi(pololetni, "pololetni.html");

    //PLAN AKCI
    temp = send_request(m_activeServer + "prehled.aspx?s=17");
    viewstate = getInput(temp, "VIEWSTATE");
    eventtarget = getInput(temp, "EVENTTARGET");
    eventvalidation = getInput(temp, "EVENTVALIDATION");

    postData.clear();
    postData.append("__VIEWSTATE=" + viewstate + "&");
    postData.append("__EVENTTARGET=" + eventtarget + "&");
    postData.append("__EVENTVALIDATION=" + eventvalidation + "&");
    postData.append("ctl00%24cphmain%24dropplan=pololetí&");
//    postData.append("ctl00%24cphmain%24Flyout0%24checksvatky=on&");
    postData.append("__EVENTARGUMENT=&");
    postData.append("__LASTFOCUS=&");
    QString planAkci = send_request(m_activeServer + "prehled.aspx?s=17", POST, postData); // plan akci je doma
    parsuj_dalsi(planAkci, "planakci.html");
}

Parser::~Parser()
{
    delete m_manager;
}
