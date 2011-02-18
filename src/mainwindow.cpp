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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parser.h"
#include "nastaveni.h"

SuplChecker::SuplChecker(QWidget *parent) :
    QMainWindow(parent)
    ,ui(new Ui::SuplChecker)
    ,isLoading(false)
{
    ui->setupUi(this);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::CustomContextMenu);
    toolbar->addAction(QIcon(":/icons/reload.png"), "Obnovit", this, SLOT(opakovat()))->setShortcut(QKeySequence("F5"));
    toolbar->addAction(QIcon(":/icons/book.png"), "Nastavit údaje", this, SLOT(udaje()))->setShortcut(QKeySequence("Ctrl+P"));
    toolbar->addAction(QIcon(":/icons/about.png"), "O programu", this, SLOT(info_o_programu()));
    toolbar->addSeparator();

    actUser = new QToolButton(this);
    actUser->setText("Nevybrán uživatel");
    actUser->setPopupMode(QToolButton::InstantPopup);
    actMenu = new QMenu(this);
    actUser->setMenu(actMenu);

    toolbar->addWidget(actUser);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(20,20));

    addToolBar(toolbar);
    QToolBar* closeToolbar = new QToolBar(this);
    closeToolbar->addAction(QIcon(":/icons/exit.png"), "Konec", this, SLOT(close()))->setShortcut(QKeySequence("Ctrl+Q"));
    closeToolbar->setLayoutDirection(Qt::RightToLeft);
    closeToolbar->setMovable(false);
    closeToolbar->setContextMenuPolicy(Qt::CustomContextMenu);
    addToolBar(closeToolbar);

#ifdef Q_WS_WIN
    toolbar->setStyleSheet("QToolBar{background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 #d4dbee); border: none; }");
    closeToolbar->setStyleSheet("QToolBar{background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 #d4dbee); border:none; }");
    ui->tabWidget->setStyleSheet("QTabBar{background-color: #d4dbee; } QTabWidget{border: none;}");
#endif

    vycentruj();
    // SQLite Database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data/checker.db");
    if (!db.open()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Chyba!");
        msgBox.setText("Nepodařilo se připojit k databázi uživatelů!");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowIcon(QIcon(":icon.png"));
        msgBox.exec();
        return;
    }

    QSqlQuery query;
    query.exec("SELECT jmeno, heslo, server FROM users ORDER by naposledy DESC");
    query.next();
    QString jmeno = query.value(0).toString();
    QString heslo = query.value(1).toString();
    QString server = query.value(2).toString();
    query.clear();

    if (jmeno=="" || heslo=="" || server=="") {
        udaje();
     } else {
        zacni_loadovat(jmeno,heslo,server);
    }
    aktShown=0;

    aktualizujUzivatele();
}

void SuplChecker::setLoading(bool set)
{
    isLoading = set;
}

void SuplChecker::vycentruj()
{
    const QRect screen=QApplication::desktop()->screenGeometry();
    const QRect &size=QWidget::geometry();
    QWidget::move( (screen.width()-size.width())/2, (screen.height()-size.height())/2 );
}

void SuplChecker::zacni_loadovat(QString uzjmeno, QString uzheslo, QString server)
{
    if (isLoading)
        return;
    actUser->setText("Načítám ...");
    spatneUdaje=0;
    if (QFile("data/tentotyden.html").exists())
    {
        QFile("data/tentotyden.html").remove();
    }
    if (QFile("data/dalsityden.html").exists())
    {
        QFile("data/dalsityden.html").remove();
    }
    if (QFile("data/stalyrozvrh.html").exists())
    {
        QFile("data/stalyrozvrh.html").remove();
    }
    if (QFile("data/znamky.html").exists())
    {
        QFile("data/znamky.html").remove();
    }

    ui->webView_1->setUrl(QUrl("data/loading.html"));
    ui->webView_2->setUrl(QUrl("data/loading.html"));
    ui->webView_3->setUrl(QUrl("data/loading.html"));
    ui->webView_4->setUrl(QUrl("data/loading.html"));

    //in new thread -> start();
//#ifndef QT_NO_DEBUG
//    qDebug() << "PAUSED FOR TESTING";
//    return;
//#endif

    QSqlQuery query;
    QDateTime now = QDateTime::currentDateTime();
    query.prepare("UPDATE users SET naposledy=? WHERE jmeno=?");
    query.bindValue(0,now.toMSecsSinceEpoch());
    query.bindValue(1,uzjmeno);
    query.exec();


    aktJmeno=uzjmeno;
    Parser *vlakno = new Parser(uzjmeno,uzheslo,server);
    connect(vlakno, SIGNAL(jmeno(QString,QString)),this, SLOT(jmeno(QString,QString)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(aktualizace(QString,QString,QString)),this, SLOT(aktualizace(QString,QString,QString)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(done(QString)),this, SLOT(nacti(QString)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(chyba(QString)),this, SLOT(chyba(QString)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(zobraz_udaje()),this, SLOT(udaje()),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(loading(bool)),this, SLOT(setLoading(bool)),Qt::QueuedConnection);
    vlakno->start();

}

void SuplChecker::info_o_programu()
{
    QMessageBox msgBox(this);
    msgBox.setText("<h1>SuplChecker 0.5</h1>Jednoduchý checker suplů a známek<br/><br/><b>Autor:</b> nowrep<br/><b>Poděkování:</b> Rajnymu a Patrickovi<br/>"
                   "<br/><small>Build time: 06/02/2011 14:44<br/>Copyright (C) 2010-2011 nowrep<br/><a href='http://suplchecker.wz.cz'>http://suplchecker.wz.cz</a>");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowIcon(QIcon(":icon.png"));
    msgBox.setIconPixmap(QPixmap(":icon.png"));
    msgBox.exec();
}

void SuplChecker::udaje()
{
    nastaveni window(this,this);
    window.exec();
}

void SuplChecker::nacti(QString info)
{
    if (spatneUdaje==0){
    if (info=="tentotyden.html")
        ui->webView_1->setUrl(QUrl("data/tentotyden.html"));
    if (info=="dalsityden.html")
        ui->webView_2->setUrl(QUrl("data/dalsityden.html"));
    if (info=="stalyrozvrh.html")
        ui->webView_3->setUrl(QUrl("data/stalyrozvrh.html"));
    if (info=="znamky.html")
        ui->webView_4->setUrl(QUrl("data/znamky.html"));
    }else if (spatneUdaje!=5){
        spatneUdaje=5;
        QMessageBox msgBox;
        msgBox.setWindowTitle("Chyba!");
        msgBox.setText("Nepodařilo se přihlásit na Váš účet!\nZkontrolujte správnost údajů, internetové připojení a dostupnost Bakalářů!");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowIcon(QIcon(":icon.png"));
        msgBox.exec();
    }
}

void SuplChecker::go()
{
    qDebug() << "go";
}

void SuplChecker::jmeno(QString jmeno, QString trida)
{
    if (jmeno!=""){
        actUser->setText(jmeno+" ("+trida+")");
    }else{
        spatneUdaje=1;
        actUser->setText("Nesprávné údaje!");
    }
}

void SuplChecker::aktualizace(QString stara, QString nova, QString changelog)
{
    if (aktShown==0){
    aktShown=1;
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Aktualizace!");
    msgBox.setText("<h2>Je dostupná aktualizace programu!</h2><b>Vaše verze: </b>"+stara+"<br/><b>Nová verze: </b>"+nova+"<br/><br/>"+changelog+"<br/><br/>"
                   "<small><a href='http://suplchecker.wz.cz/download.php'>Novou verzi stáhnete zde!</a></small>");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowIcon(QIcon(":icon.png"));
    msgBox.exec();
    }
}

void SuplChecker::chyba(QString text)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Chyba!");
    msgBox.setText("Při zpracování suplování došlo k neočekáváné chybě:\n"+text);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowIcon(QIcon(":icon.png"));
    msgBox.exec();
}

void SuplChecker::opakovat()
{
    vybrano(aktJmeno);
}

void SuplChecker::aktualizujUzivatele()
{
    actMenu->clear();
    QSqlQuery query;
    query.exec("SELECT jmeno FROM users ORDER BY naposledy");
    while(query.next()){
        actMenu->addAction("Načíst uživatele "+query.value(0).toString(), this, SLOT(loadAction()))->setData(query.value(0).toString());
    }
}

void SuplChecker::vybrano(QString text)
{
    QSqlQuery query;
    query.exec("SELECT jmeno, heslo, server FROM users WHERE jmeno='"+text+"'");
    query.next();
    QString jmeno = query.value(0).toString();
    QString heslo = query.value(1).toString();
    QString server = query.value(2).toString();

    zacni_loadovat(jmeno,heslo,server);
}

void SuplChecker::loadAction()
{
    if (QAction *action = qobject_cast<QAction*>(sender()))
        vybrano(action->data().toString());
}

SuplChecker::~SuplChecker()
{
    delete ui;
}
