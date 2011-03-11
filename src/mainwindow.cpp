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
#include "parser.h"
#include "nastaveni.h"

#ifdef Q_OS_SYMBIAN
#include "ui_mainwindowS60.h"
#else
#include "ui_mainwindow.h"
#endif

SuplChecker::SuplChecker(QWidget *parent) :
    QMainWindow(parent)
    ,DATADIR(
#ifdef Q_OS_SYMBIAN
        "e://data/suplchecker/"
#else
        qApp->applicationDirPath()+"/data/"
#endif
)
    ,ui(new Ui::SuplChecker)
    ,aktShown(false)
    ,isLoading(false)
{
    QTranslator translator;
    translator.load(DATADIR+"qt_cs.qm");
    qApp->installTranslator(&translator);

    ui->setupUi(this);
#ifdef Q_OS_SYMBIAN
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowSoftkeysVisibleHint;
    flags &= ~Qt::WindowSoftkeysRespondHint;
    setWindowFlags(flags);
    showFullScreen();

    QMenu* menuShow = new QMenu("Zobrazit...");
    menuShow->addAction("Zobrazit tento týden", this, SLOT(showPageOnSymbian()))->setData(SuplChecker::CurrentWeek);
    menuShow->addAction("Zobrazit příští týden", this, SLOT(showPageOnSymbian()))->setData(SuplChecker::NextWeek);
    menuShow->addAction("Zobrazit stálý rozvrh", this, SLOT(showPageOnSymbian()))->setData(SuplChecker::Permanent);
    menuShow->addAction("Zobrazit hodnocení", this, SLOT(showPageOnSymbian()))->setData(SuplChecker::Marks);
    menuBar()->addMenu(menuShow);
    menuBar()->addAction("O programu", this, SLOT(info_o_programu()));
    menuBar()->addAction("Konec", this, SLOT(close()));

    zacni_loadovat("rosca0o0", "33bty23v", "http://g8mb.cz/bakaweb/");
#else
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
#endif

    aktualizujUzivatele();
    vycentruj();
}

void SuplChecker::setLoading(bool set)
{
    isLoading = set;
}

void SuplChecker::vycentruj()
{
#ifndef Q_OS_SYMBIAN
    const QRect screen=QApplication::desktop()->screenGeometry();
    const QRect &size=QWidget::geometry();
    QWidget::move( (screen.width()-size.width())/2, (screen.height()-size.height())/2 );
#endif
}

void SuplChecker::zacni_loadovat(QString uzjmeno, QString uzheslo, QString server)
{
    if (isLoading)
        return;

#ifndef Q_OS_SYMBIAN
    QSqlQuery query;
    QDateTime now = QDateTime::currentDateTime();
    query.prepare("UPDATE users SET naposledy=? WHERE jmeno=?");
    query.bindValue(0,now.toMSecsSinceEpoch());
    query.bindValue(1,uzjmeno);
    query.exec();

    actUser->setText("Načítám ...");
#endif
    spatneUdaje=0;

    ui->webView_1->setUrl(QUrl::fromLocalFile(DATADIR+"loading.html"));
    ui->webView_2->setUrl(QUrl::fromLocalFile(DATADIR+"loading.html"));
    ui->webView_3->setUrl(QUrl::fromLocalFile(DATADIR+"loading.html"));
    ui->webView_4->setUrl(QUrl::fromLocalFile(DATADIR+"loading.html"));

    //in new thread -> start();
//#ifndef QT_NO_DEBUG
//    qDebug() << "PAUSED FOR TESTING";
//    return;
//#endif
    aktJmeno=uzjmeno;
    Parser *vlakno = new Parser(uzjmeno,uzheslo,server);
    connect(vlakno, SIGNAL(jmeno(QString,QString)),this, SLOT(jmeno(QString,QString)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(aktualizace(QString,QString,QString)),this, SLOT(aktualizace(QString,QString,QString)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(done(QString,QByteArray)),this, SLOT(nacti(QString,QByteArray)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(chyba(QString)),this, SLOT(chyba(QString)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(zobraz_udaje()),this, SLOT(udaje()),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(loading(bool)),this, SLOT(setLoading(bool)),Qt::QueuedConnection);
    vlakno->start();

}

void SuplChecker::info_o_programu()
{
    QMessageBox msgBox(this);
#ifdef Q_OS_SYMBIAN
    msgBox.setText("SuplChecker 0.5<small><br/>Checker suplů a známek<br/>Autor: nowrep<br/>(C) 2010-2011 nowrep<br/><a href='http://suplchecker.wz.cz'>http://suplchecker.wz.cz</a></small>");
#else
    msgBox.setText("<h1>SuplChecker 0.5</h1>Jednoduchý checker suplů a známek<br/><br/><b>Autor:</b> nowrep<br/><b>Poděkování:</b> Rajnymu a Patrickovi<br/>"
                   "<br/><small>Build time: 06/02/2011 14:44<br/>Copyright (C) 2010-2011 nowrep<br/><a href='http://suplchecker.wz.cz'>http://suplchecker.wz.cz</a>");
#endif
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

void SuplChecker::nacti(QString info, QByteArray data)
{
    if (spatneUdaje==0){
    if (info=="tentotyden.html")
        ui->webView_1->page()->mainFrame()->setContent(data, QString(), QUrl::fromLocalFile(DATADIR));
    if (info=="dalsityden.html")
        ui->webView_2->page()->mainFrame()->setContent(data, QString(), QUrl::fromLocalFile(DATADIR));
    if (info=="stalyrozvrh.html")
        ui->webView_3->page()->mainFrame()->setContent(data, QString(), QUrl::fromLocalFile(DATADIR));
    if (info=="znamky.html")
        ui->webView_4->page()->mainFrame()->setContent(data, QString(), QUrl::fromLocalFile(DATADIR));
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
#ifndef Q_OS_SYMBIAN
    if (jmeno!=""){
        actUser->setText(jmeno+" ("+trida+")");
    }else{
        spatneUdaje=1;
        actUser->setText("Nesprávné údaje!");
    }
#else
    Q_UNUSED(jmeno)
    Q_UNUSED(trida)
#endif
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
#ifndef Q_OS_SYMBIAN
    actMenu->clear();
    QSqlQuery query;
    query.exec("SELECT jmeno FROM users ORDER BY naposledy");
    while(query.next()){
        actMenu->addAction("Načíst uživatele "+query.value(0).toString(), this, SLOT(loadAction()))->setData(query.value(0).toString());
    }
#endif
}

void SuplChecker::vybrano(QString text)
{
#ifndef Q_OS_SYMBIAN
    QSqlQuery query;
    query.exec("SELECT jmeno, heslo, server FROM users WHERE jmeno='"+text+"'");
    query.next();
    QString jmeno = query.value(0).toString();
    QString heslo = query.value(1).toString();
    QString server = query.value(2).toString();

    zacni_loadovat(jmeno,heslo,server);
#else
    Q_UNUSED(text)
#endif
}

void SuplChecker::loadAction()
{
    if (QAction *action = qobject_cast<QAction*>(sender()))
        vybrano(action->data().toString());
}

void SuplChecker::showPageOnSymbian()
{
#ifdef Q_OS_SYMBIAN
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        switch (action->data().toInt()) {
        case SuplChecker::CurrentWeek:
            ui->stackedWidget->setCurrentIndex(0);
            break;
        case SuplChecker::NextWeek:
            ui->stackedWidget->setCurrentIndex(1);
            break;
        case SuplChecker::Permanent:
            ui->stackedWidget->setCurrentIndex(2);
            break;
        case SuplChecker::Marks:
            ui->stackedWidget->setCurrentIndex(3);
            break;
        }
    }
#endif
}

SuplChecker::~SuplChecker()
{
    delete ui;
}
