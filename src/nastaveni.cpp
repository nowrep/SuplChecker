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
#include "nastaveni.h"
#include "ui_nastaveni.h"

nastaveni::nastaveni(MainWindow* mainWindow,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::nastaveni)
{
    ui->setupUi(this);

    QString jmeno=""; //uzivatelske jmeno
    QString heslo=""; //uzivatelske heslo
    QString server=""; //server na ktery se chceme prihlasit

    mainwindow=mainWindow;

    QSqlQuery query;
    query.exec("SELECT jmeno, heslo, server FROM users ORDER BY naposledy DESC");
    query.next();
    jmeno=query.value(0).toString();
    heslo=query.value(1).toString();
    server=query.value(2).toString();

    aktJmeno=jmeno;

    ui->jmeno->setText(jmeno);
    ui->heslo->setText(heslo);

    if (server=="http://g8mb.cz/bakaweb/"){
        ui->server->setCurrentIndex(0);
    }else if (server=="http://gserver/bakaweb/"){
        ui->server->setCurrentIndex(1);
    }else{
        ui->server->setCurrentIndex(0);
    }
    aktualizuj_tabulku();

    connect(ui->buttonBox2, SIGNAL(accepted()), this, SLOT(nacti()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(zapis_udaje()));

    connect(ui->treeWidget, SIGNAL(itemClicked (QTreeWidgetItem *, int)), this, SLOT(zobraz_kliknuty(QTreeWidgetItem *,int)));
    connect(ui->butNovy, SIGNAL(clicked()), this, SLOT(pridat_uziv()));
    connect(ui->butSmazat, SIGNAL(clicked()), this, SLOT(smazat_uziv()));
}

void nastaveni::zapis_udaje()
{
    QString jmeno=ui->jmeno->text();
    QString heslo=ui->heslo->text();
    QString server=ui->server->currentText();

    if (jmeno=="" || heslo=="" || server==""){
        QMessageBox msgBox;
        msgBox.setText("Nevyplnil jsi jméno a heslo!");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowIcon(QIcon(":icon.png"));
        msgBox.exec();
    }else{
    QSqlQuery query;
    query.prepare("UPDATE users SET jmeno=?, heslo=?, server=? WHERE jmeno=?");
    query.bindValue(0,jmeno);
    query.bindValue(1,heslo);
    query.bindValue(2,server);
    query.bindValue(3,aktJmeno);
    query.exec();
    aktJmeno=jmeno;
    ui->info->setText("Uživatel "+jmeno+" uložen.");
    aktualizuj_tabulku();
    if (jmeno.length()!=8 || heslo.length()!=8){
        ui->info->setText("Jméno a heslo by mělo mít 8 znaků (podle původních loginů)!");
    }
    }
}

void nastaveni::aktualizuj_tabulku()
{
    int i=0;
    ui->treeWidget->clear();
    QSqlQuery query;
    query.exec("SELECT jmeno, heslo, server FROM users ORDER by naposledy");
    while (query.next()){
        QTreeWidgetItem *item=new QTreeWidgetItem(ui->treeWidget);
        item->setText(0,query.value(0).toString());
        item->setText(1,query.value(1).toString());
        item->setText(2,query.value(2).toString());
        ui->treeWidget->addTopLevelItem(item);
        i++;
    }
    if (i==0){
        pridat_uziv();
        ui->jmeno->setText("Nový");
        ui->heslo->setText("-");
        aktJmeno="Nový";
    }
}

void nastaveni::pridat_uziv()
{
    QSqlQuery query;
    query.exec("SELECT id FROM users WHERE jmeno='Nový'");

    if (!query.next()){
    query.exec("INSERT INTO users (jmeno,heslo,server) VALUES('Nový','-','')");
    aktualizuj_tabulku();
    ui->info->setText("Nový uživatel přidán.");
    }else{
    ui->info->setText("Nejdříve upravte nového uživatele!");
    }
}

void nastaveni::smazat_uziv()
{
    QString akt=ui->treeWidget->currentItem()->text(0);
    QSqlQuery query;
    query.exec("DELETE FROM users WHERE jmeno='"+akt+"'");
    ui->info->setText("Uživatel "+akt+" byl smazán.");
    ui->jmeno->clear();
    ui->heslo->clear();
    aktualizuj_tabulku();
}

void nastaveni::zobraz_kliknuty(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    QSqlQuery query;
    query.prepare("SELECT jmeno,heslo,server FROM users WHERE jmeno=?");
    query.bindValue(0,item->text(0));
    query.exec();
    query.next();
    ui->jmeno->setText(query.value(0).toString());
    ui->heslo->setText(query.value(1).toString());
    aktJmeno=ui->jmeno->text();
}

void nastaveni::nacti()
{
    zapis_udaje();
    QString jmeno=ui->jmeno->text();
    QString heslo=ui->heslo->text();
    QString server=ui->server->currentText();
    if (jmeno!=""){
    mainwindow->zacni_loadovat(jmeno,heslo,server);
    close();
    }
}

nastaveni::~nastaveni()
{
    delete ui;
}
