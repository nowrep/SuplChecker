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
#include "parser.h"

Parser::Parser(QString jmeno, QString heslo, QString server) : QThread()
{
  uzJmeno=jmeno;
  uzHeslo=heslo;
  uzServer=server;
  moveToThread(this);
}

QString Parser::vrat_den(int den, QString zdroj)
{
    QString vyraz;
    switch (den){
        case 1:vyraz="<tr><td class=\"rozden\" Rowspan=3>Po(.*)<tr><td class=\"rozden\" Rowspan=3>";break;
        case 2:vyraz="<tr><td class=\"rozden\" Rowspan=3>Út(.*)<tr><td class=\"rozden\" Rowspan=3>";break;
        case 3:vyraz="<tr><td class=\"rozden\" Rowspan=3>St(.*)<tr><td class=\"rozden\" Rowspan=3>";break;
        case 4:vyraz="<tr><td class=\"rozden\" Rowspan=3>Čt(.*)<tr><td class=\"rozden\" Rowspan=3>";break;
        case 5:vyraz="<tr><td class=\"rozden\" Rowspan=3>Pá(.*)</tbody></table>";break;
    }

    QRegExp rx(vyraz, Qt::CaseInsensitive );
    rx.setMinimal(true);
    rx.indexIn(zdroj);
    QString captured = rx.cap(1);
    switch (den){
        case 1: captured="<tr><td class=\"rozden\" Rowspan=3>Po"+captured;break;
        case 2: captured="<tr><td class=\"rozden\" Rowspan=3>Út"+captured;break;
        case 3: captured="<tr><td class=\"rozden\" Rowspan=3>St"+captured;break;
        case 4: captured="<tr><td class=\"rozden\" Rowspan=3>Čt"+captured;break;
        case 5: captured="<tr><td class=\"rozden\" Rowspan=3>Pá"+captured;break;
    }

    if (rx.indexIn(zdroj)){
        return captured;
    }
    else{
        return ""; // den nebyl nalezen -> bug?
    }
}

QString Parser::vrat_tyden(QString zdroj)
{
    QString vyraz="<div class=\"rozpopishod\">(.*)</div>";
    QRegExp rx(vyraz, Qt::CaseInsensitive );
    rx.setMinimal(true);
    rx.indexIn(zdroj);
    QString od = rx.cap(1);
    rx.lastIndexIn(zdroj);
    QString dod = rx.cap(1);
    return "<center><h1>"+od+" - "+dod+" "+QDateTime::currentDateTime().toString("yyyy")+"</center></h1>";
}

bool Parser::zjisti_supl(QString zdroj)
{
    QString vyraz="<td class=\"rozpredmet1zm\" ";
    QString vyraz2="<td class=\"rozpredmet1abs\" ";
    if (zdroj.contains(vyraz) || zdroj.contains(vyraz2)){
        return true;
    }
    else{
        return false;
    }
}

QString Parser::checkni_tyden(QString zdroj)
{
    QString pondeli=vrat_den(1,zdroj);
    QString utery=vrat_den(2,zdroj);
    QString streda=vrat_den(3,zdroj);
    QString ctvrtek=vrat_den(4,zdroj);
    QString patek=vrat_den(5,zdroj);

    bool po_supl=zjisti_supl(pondeli);
    bool ut_supl=zjisti_supl(utery);
    bool st_supl=zjisti_supl(streda);
    bool ct_supl=zjisti_supl(ctvrtek);
    bool pa_supl=zjisti_supl(patek);

    QString vystup="";
    if (po_supl){
        vystup+=pondeli+"\n";
    }
    if (ut_supl){
        vystup+=utery+"\n";
    }
    if (st_supl){
        vystup+=streda+"\n";
    }
    if (ct_supl){
        vystup+=ctvrtek+"\n";
    }
    if (pa_supl){
        vystup+=patek+"\n";
    }

    return vystup;
}

QString Parser::send_request(QNetworkAccessManager *manager, QString url, int method, QByteArray postData)
{
    QNetworkReply* reply;
    if (method==1){
    reply=manager->get(QNetworkRequest(QUrl(url)));
    }
    else{
        reply=manager->post(QNetworkRequest(QUrl(url)),postData);
    }
    QEventLoop eLoop;
    connect( manager, SIGNAL( finished( QNetworkReply * ) ), &eLoop, SLOT(quit() ) );
    eLoop.exec();
    return reply->readAll();
}

void Parser::parsuj_tyden(QString zdroj,QString soubor)
{
    QString zac_html="<html><head><link rel='stylesheet' href='styl.css' type='text/css' media='screen' /><meta http-equiv='content-type' content='text/html; charset=utf-8'></head><body>";

    QString vyraz="<table cellspacing=\"0\" cellpadding=\"0\" class=\"rozbunka\" >(.*)</td></tr>(.*)</td></tr>";
    QRegExp rx(vyraz, Qt::CaseInsensitive );
    rx.setMinimal(true);
    rx.indexIn(zdroj);
    QString zac_tabulky = rx.cap(0).replace("<br />","-");

    QString temp=checkni_tyden(zdroj);
    if (temp==""){
        temp="<center>V požadovanou dobu nenalezeno žádné suplování!</center>";
        zac_tabulky="";
    }
    QString datum=vrat_tyden(zdroj);
//    QFile file("data/"+soubor);
//    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QByteArray data;
    data.append(zac_html);
    data.append(zac_tabulky);
    data.append(datum);
    data.append(temp);
    data.append("</tbody></table></body></html>");
//    file.write(data);
//    file.close();

    emit done(soubor, data);
}

void Parser::vrat_jmeno(QString zdroj)
{
    QString vyraz="<td style=\"width:100%;\" class=\"logjmeno\" colspan=\"2\">(.*), (.*)</td>";
    QRegExp rx(vyraz, Qt::CaseInsensitive );
    rx.setMinimal(true);
    rx.indexIn(zdroj);
    //QString captured = rx.cap(0);
    //QString vysledek=rx.cap(1)+"("+rx.cap(2)+")";
    emit jmeno(rx.cap(1),rx.cap(2));
}

void Parser::parsuj_dalsi(QString zdroj,QString soubor)
{
    QString zac_html="<html><head><link rel='stylesheet' href='styl.css' type='text/css' media='screen' /><meta http-equiv='content-type' content='text/html; charset=utf-8'></head><body>";
    QString kon_html="</body></html>";

    QString kon_tabulky="</tbody></table>";
    if (soubor=="stalyrozvrh.html"){
        QString vyraz="<table cellspacing=\"0\" cellpadding=\"0\" class=\"rozbunka\" >(.*)\n";
        QRegExp rx(vyraz, Qt::CaseInsensitive );
        rx.setMinimal(true);
        rx.indexIn(zdroj);
        QString captured = rx.cap(0);

//        QFile file("data/"+soubor);
//        file.open(QIODevice::ReadWrite | QIODevice::Text);
        QByteArray data;
        data.append(zac_html);
        data.append("<center><h1>Stálý rozvrh</h1></center>");
        data.append(captured);
        data.append(kon_tabulky);
        data.append(kon_html);
//        file.write(data);
//        file.close();

        emit done(soubor, data);
    }
    else if (soubor=="znamky.html"){
        QString vyraz="<table class=\"radekznamky\">(.*)</tbody></table>";
        QRegExp rx(vyraz, Qt::CaseInsensitive );
        rx.setMinimal(true);
        rx.indexIn(zdroj);
        QString captured = rx.cap(0);

//        QFile file("data/"+soubor);
//        file.open(QIODevice::ReadWrite | QIODevice::Text);
        QByteArray data;
        data.append(zac_html);
        data.append("<link rel='stylesheet' href='basicstyl.css' type='text/css' media='screen' /><center><h1>Průběžná klasifikace</h1>");
        data.append(captured);
        data.append(kon_tabulky);
        data.append(kon_html);
//        file.write(data);
//        file.close();

        emit done(soubor, data);
    }
}

QString Parser::getInput(QString zdroj, QString input)
{
    QString vyraz="<input type=\"hidden\" name=\"__"+input+"\" id=\"__"+input+"\" value=\"(.*)\"";
    QRegExp rx(vyraz, Qt::CaseInsensitive );
    rx.setMinimal(true);
    rx.indexIn(zdroj);
    QString vysl=rx.cap(1);
    QByteArray temp;
    temp.append(vysl);
    QString vysledek = temp.toPercentEncoding();
    return vysledek;
}

void Parser::run()
{
  QTimer::singleShot(0, this, SLOT(pracuj()));
  exec();
}

void Parser::pracuj()
{
    emit loading(true);
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QByteArray postData;
    postData.append("ctl00$cphmain$TextBoxjmeno="+uzJmeno+"&");
    postData.append("ctl00$cphmain$TextBoxheslo="+uzHeslo+"&");
    postData.append("__VIEWSTATE=&");
    postData.append("ctl00$cphmain$ButtonPrihlas=P%C5%99ihl%C3%A1sit&");
    send_request(manager,uzServer+"login.aspx",2, postData);
    QString tento_tyden=send_request(manager,uzServer+"prehled.aspx?s=6"); // tento tyden je doma
    vrat_jmeno(tento_tyden);
    parsuj_tyden(tento_tyden,"tentotyden.html"); // :-)

    ///PRISTI TYDEN
    QString viewstate=getInput(tento_tyden,"VIEWSTATE");
    QString eventtarget=getInput(tento_tyden,"EVENTTARGET");
    QString eventvalidation=getInput(tento_tyden,"EVENTVALIDATION");

    postData.clear();
    postData.append("__EVENTTARGET="+eventtarget+"&");
    postData.append("__EVENTVALIDATION="+eventvalidation+"&");
    postData.append("__VIEWSTATE="+viewstate+"&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24checkucitel=on&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24checkskupina=on&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24Checkmistnost=on&");
    postData.append("ctl00%24cphmain%24radiorozvrh=rozvrh+na+p%C5%99%C3%AD%C5%A1t%C3%AD+t%C3%BDden");
    QString pristi_tyden=send_request(manager,uzServer+"prehled.aspx?s=6",2,postData); // pristi tyden je doma
    parsuj_tyden(pristi_tyden,"dalsityden.html");

    ///STALY ROZVRH
    viewstate=getInput(pristi_tyden,"VIEWSTATE");
    eventtarget=getInput(pristi_tyden,"EVENTTARGET");
    eventvalidation=getInput(pristi_tyden,"EVENTVALIDATION");

    postData.clear();
    postData.append("__EVENTTARGET="+eventtarget+"&");
    postData.append("__EVENTVALIDATION="+eventvalidation+"&");
    postData.append("__VIEWSTATE="+viewstate+"&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24checkucitel=on&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24checkskupina=on&");
    postData.append("ctl00%24cphmain%24Flyrozvrh%24Checkmistnost=on&");
    postData.append("ctl00%24cphmain%24radiorozvrh=st%C3%A1l%C3%BD+rozvrh");
    QString staly_rozvrh=send_request(manager,uzServer+"prehled.aspx?s=6",2,postData); // staly rozvrh je doma
    parsuj_dalsi(staly_rozvrh,"stalyrozvrh.html");

    ///ZNAMKY
    QString temp=send_request(manager,uzServer+"prehled.aspx?s=2"); //
    viewstate=getInput(temp,"VIEWSTATE");
    eventtarget=getInput(temp,"EVENTTARGET");
    eventvalidation=getInput(temp,"EVENTVALIDATION");

    postData.clear();
    postData.append("__VIEWSTATE="+viewstate+"&");
    postData.append("__EVENTTARGET="+eventtarget+"&");
    postData.append("__EVENTVALIDATION="+eventvalidation+"&");
    postData.append("__EVENTARGUMENT=&");
    postData.append("__LASTFOCUS=&");
    postData.append("ctl00%24cphmain%24Flyout2%24Checkprumery=on&");
    postData.append("ctl00%24cphmain%24listdoba=pololeti&");
    QString znamky=send_request(manager,uzServer+"prehled.aspx?s=2",2,postData); // znamky jsou doma
    parsuj_dalsi(znamky,"znamky.html");

    //Check new version::
    QString current_version="0.5 (6.2.2011)";
    QString actual_version=send_request(manager,"http://suplchecker.wz.cz/version.php");

    if (!actual_version.contains(current_version) && actual_version.contains("Aktuální verze: ")){
        QString changelog=send_request(manager,"http://suplchecker.wz.cz/changelog.php");
        emit aktualizace(current_version,actual_version.replace("Aktuální verze: ",""),changelog);
    }
    //else->file.open
    emit loading(false);
}
