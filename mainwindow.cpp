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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCornerWidget(ui->comJmeno);
    ui->comJmeno->setHidden(true);
    ui->tabWidget->setCornerWidget(ui->butJmeno);

    vycentruj();
    // SQLite Database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("checker.db");
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

    if (jmeno=="" || heslo=="" || server==""){
        udaje();
    }else{
    QDateTime now = QDateTime::currentDateTime();
    query.prepare("UPDATE users SET naposledy=? WHERE jmeno=?");
    query.bindValue(0,now.toMSecsSinceEpoch());
    query.bindValue(1,jmeno);
    query.exec();
    zacni_loadovat(jmeno,heslo,server);
    }
    aktShown=0;

    connect(ui->actionOpakovat, SIGNAL(triggered()), this, SLOT(opakovat()));
    connect(ui->actionO_programu, SIGNAL(triggered()), this, SLOT(info_o_programu()));

    connect(ui->butJmeno, SIGNAL(clicked()), this, SLOT(zobraz_vyber()));
    connect(ui->actionUdaje, SIGNAL(triggered()), this, SLOT(udaje()));
    connect(ui->actionZavrit, SIGNAL(triggered()), this, SLOT(close()));

}

void MainWindow::vycentruj()
{
    const QRect screen=QApplication::desktop()->screenGeometry();
    const QRect &size=QWidget::geometry();
    QWidget::move( (screen.width()-size.width())/2, (screen.height()-size.height())/2 );
}

void MainWindow::zacni_loadovat(QString uzjmeno, QString uzheslo, QString server)
{
    ui->butJmeno->setText("Načítám ...");
    spatneUdaje=0;
    if (QFile("tentotyden.html").exists())
    {
        QFile("tentotyden.html").remove();
    }
    if (QFile("dalsityden.html").exists())
    {
        QFile("dalsityden.html").remove();
    }
    if (QFile("stalyrozvrh.html").exists())
    {
        QFile("stalyrozvrh.html").remove();
    }
    if (QFile("znamky.html").exists())
    {
        QFile("znamky.html").remove();
    }

    ui->webView_1->setUrl(QUrl("loading.html"));
    ui->webView_2->setUrl(QUrl("loading.html"));
    ui->webView_3->setUrl(QUrl("loading.html"));
    ui->webView_4->setUrl(QUrl("loading.html"));

    //in new thread -> start();
    aktJmeno=uzjmeno;
    Parser *vlakno = new Parser(uzjmeno,uzheslo,server);
    connect(vlakno, SIGNAL(jmeno(QString,QString)),this, SLOT(jmeno(QString,QString)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(aktualizace(QString,QString,QString)),this, SLOT(aktualizace(QString,QString,QString)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(done(QString)),this, SLOT(nacti(QString)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(chyba(QString)),this, SLOT(chyba(QString)),Qt::QueuedConnection);
    connect(vlakno, SIGNAL(zobraz_udaje()),this, SLOT(udaje()),Qt::QueuedConnection);
    vlakno->start();

}

void MainWindow::info_o_programu()
{
    QMessageBox msgBox;
    msgBox.setText("<h1>SuplChecker 0.2</h1>Jednoduchý checker suplů a známek<br/><br/><b>Autor:</b> davnowrep/><b>Poděkování:</b> Rajnymu a Patrickovi<br/><br/><small>Build time: 06/12/2010 20:57<br/>Copyright (C) 2010-2011 nowrep<a href='http://suplchecker.wz.cz'>http://suplchecker.wz.cz</a>");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowIcon(QIcon(":icon.png"));
    msgBox.setIconPixmap(QPixmap(":icon.png"));
    msgBox.exec();
}

void MainWindow::udaje()
{
    nastaveni window(this);
    window.exec();
}

void MainWindow::nacti(QString info)
{
    if (spatneUdaje==0){
    if (info=="tentotyden.html")
        ui->webView_1->setUrl(QUrl("tentotyden.html"));
    if (info=="dalsityden.html")
        ui->webView_2->setUrl(QUrl("dalsityden.html"));
    if (info=="stalyrozvrh.html")
        ui->webView_3->setUrl(QUrl("stalyrozvrh.html"));
    if (info=="znamky.html")
        ui->webView_4->setUrl(QUrl("znamky.html"));
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

void MainWindow::go()
{
    qDebug() << "go";
}

void MainWindow::jmeno(QString jmeno, QString trida)
{
    if (jmeno!=""){
        ui->butJmeno->setText(jmeno+" ("+trida+")");
    }else{
        spatneUdaje=1;
        ui->butJmeno->setText("Nesprávné údaje!");
    }
}

void MainWindow::aktualizace(QString stara, QString nova, QString changelog)
{
    if (aktShown==0){
    aktShown=1;
    QMessageBox msgBox;
    msgBox.setWindowTitle("Aktualizace!");
    msgBox.setText("<h2>Je dostupná aktualizace programu!</h2><b>Vaše verze: </b>"+stara+"<br/><b>Nová verze: </b>"+nova+"<br/><br/>"+changelog+"<br/><br/><small><a href='http://suplchecker.wz.cz/download.php'>Novou verzi stáhnete zde!</a></small>");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowIcon(QIcon(":icon.png"));
    msgBox.exec();
    }
}

void MainWindow::chyba(QString text)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Chyba!");
    msgBox.setText("Při zpracování suplování došlo k neočekáváné chybě:\n"+text);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowIcon(QIcon(":icon.png"));
    msgBox.exec();
}

void MainWindow::opakovat()
{
    vybrano(aktJmeno);
}

void MainWindow::zobraz_vyber()
{
        ui->butJmeno->setHidden(true);
        ui->comJmeno->setHidden(false);
        ui->tabWidget->setCornerWidget(ui->comJmeno);

        ui->comJmeno->clear();

        if (aktJmeno!=""){
        ui->comJmeno->insertItem(0,aktJmeno);
        }
        QSqlQuery query;
        query.exec("SELECT jmeno FROM users WHERE jmeno<>'"+aktJmeno+"'ORDER BY naposledy");
        while(query.next()){
            ui->comJmeno->insertItem(0,query.value(0).toString());
        }
        connect(ui->comJmeno, SIGNAL(currentIndexChanged ( const QString &)), this, SLOT(vybrano(QString)));
}

void MainWindow::vybrano(QString text)
{
    QSqlQuery query;
    query.exec("SELECT jmeno, heslo, server FROM users WHERE jmeno='"+text+"'");
    query.next();
    QString jmeno = query.value(0).toString();
    QString heslo = query.value(1).toString();
    QString server = query.value(2).toString();

    ui->comJmeno->setHidden(true);
    disconnect(ui->comJmeno, SIGNAL(currentIndexChanged ( const QString &)), this, SLOT(vybrano(QString)));
    ui->butJmeno->setHidden(false);
    ui->tabWidget->setCornerWidget(ui->butJmeno);
    ui->butJmeno->setText("Načítám...");
    zacni_loadovat(jmeno,heslo,server);
}

MainWindow::~MainWindow()
{
    delete ui;
}
