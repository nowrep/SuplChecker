/* ============================================================
* SuplChecker - simple program to check a teacher's absencies at school
* Copyright (C) 2010-2012  David Rosca <david@rosca.cz>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "suplchecker.h"
#include "parser.h"
#include "ui_mainwindow.h"
#include "nastaveni.h"
#include "globalsettings.h"
#include "qtwin.h"
#include "aboutdialog.h"
#include "ui_erroroverlay.h"
#include "globalfunctions.h"
#include "updatechecker.h"
#include "toolbar.h"

const QString SuplChecker::VERSION = "1.0.1";
const QString SuplChecker::BUILDTIME = __DATE__" "__TIME__;
const QString SuplChecker::AUTHOR = "David Rosca";
const QString SuplChecker::COPYRIGHT = "2010-2012";
const QString SuplChecker::WWWADDRESS = "http://suplchecker.wz.cz";

#define CHECK_THREAD Parser* thread = qobject_cast<Parser*>(sender()); if (!thread || thread != m_threadParser) return

QIcon SuplChecker::suplcheckerIcon()
{
    QIcon i;
    i.addFile(":icons/exeicons/suplchecker16.png");
    i.addFile(":icons/exeicons/suplchecker32.png");
    i.addFile(":icons/exeicons/suplchecker48.png");
    i.addFile(":icons/exeicons/suplchecker64.png");
    i.addFile(":icons/exeicons/suplchecker128.png");
    i.addFile(":icons/exeicons/suplchecker256.png");
    return i;
}

SuplChecker::SuplChecker(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::SuplChecker)
    , m_aboutDialog(0)
    , m_threadParser(0)
    , m_isLoading(false)
{
    GlobalSettings::loadSettings();

    QTranslator* translator = new QTranslator();
    translator->load(GlobalSettings::DataDir + "qt_cs.qm");
    qApp->installTranslator(translator);

    ui->setupUi(this);
    QApplication::setWindowIcon(suplcheckerIcon());

    if (QtWin::isCompositionEnabled()) {
        setContentsMargins(0, 0, 0, 0);
        QtWin::extendFrameIntoClientArea(this);
    }

    setStyleSheet("QToolBar {border:none; background:transparent;}"
                  "QToolButton { border-image: url(':/icons/toolbar/button.png'); border-width:3; min-width:22px; height: 19px;}"
                  "QToolButton::hover {border-image: url(':/icons/toolbar/button-hover.png');}"
                  "QToolButton::pressed {border-image: url(':/icons/toolbar/button-pressed.png');}"
                  "QToolButton::menu-indicator { image: url(:/icons/transp.png);}");

    ToolBar* leftToolbar = new ToolBar(this);
    leftToolbar->setContextMenuPolicy(Qt::CustomContextMenu);
    QWidget* spa = new QWidget();
    spa->setMinimumWidth(190);
    leftToolbar->addWidget(spa);
    leftToolbar->addAction(QIcon(":/icons/reload.png"), "Obnovit", this, SLOT(reloadWithSameUser()))->setShortcut(QKeySequence("F5"));
    leftToolbar->addAction(QIcon(":/icons/book.png"), "Nastavení", this, SLOT(showSettingsDialog()))->setShortcut(QKeySequence("Ctrl+P"));
    leftToolbar->addAction(QIcon(":/icons/about.png"), "O programu", this, SLOT(aboutProgram()));

    m_usersButton = new QToolButton(this);
    m_usersButton->setText("Nevybrán uživatel");
    m_usersButton->setToolTip("Vyberte uživatele kterého chcete načíst");
    m_usersButton->setPopupMode(QToolButton::InstantPopup);
    m_usersMenu = new QMenu(this);
    m_usersButton->setMenu(m_usersMenu);

    leftToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    leftToolbar->setMovable(false);
    leftToolbar->setIconSize(QSize(20, 20));
    addToolBar(leftToolbar);

    ToolBar* rightToolbar = new ToolBar(this);
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightToolbar->addWidget(spacer);
    rightToolbar->addWidget(m_usersButton);

    rightToolbar->setMovable(false);
    rightToolbar->setContextMenuPolicy(Qt::CustomContextMenu);
    addToolBar(rightToolbar);

    new QShortcut(QKeySequence("Ctrl+Q"), this, SLOT(close()));

    if (GlobalSettings::StartupUser.name == "jmeno") {
        QTimer::singleShot(0, this, SLOT(errorNoStartupUser()));
    }
    else {
        startLoading(GlobalSettings::StartupUser);
    }

    sc_centerWidgetOnScreen(this);

    connect(m_usersMenu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowUsersMenu()));

    if (GlobalSettings::CheckUpdates) {
        QTimer::singleShot(10000, this, SLOT(checkUpdates()));
    }

    QDate current = QDate::currentDate();
    if (current.dayOfWeek() == 6 || current.dayOfWeek() == 7) {
        ui->tabWidget->setCurrentIndex(1);
    }
}

void SuplChecker::errorNoStartupUser()
{
    chyba(Parser::NoStartupUser);
}

void SuplChecker::checkUpdates()
{
    UpdateChecker* c = new UpdateChecker;
    connect(c, SIGNAL(updateAvailable(QString, QString)), this, SLOT(aktualizace(QString, QString)));
    c->start();
}

void SuplChecker::startLoading(GlobalSettings::User user)
{
    if (m_errorFrame) {
        delete m_errorFrame;
    }

    m_usersButton->setText("Načítám ...");
    m_activeUser = user;

    QByteArray html = sc_readAllFileContents(":html/loading.html");
    html.replace("%BG-IMG-NAME%", GlobalSettings::BackgroundPixmapName.toAscii());

    ui->webView_1->setContent(html);
    ui->webView_2->setContent(html);
    ui->webView_3->setContent(html);
    ui->webView_4->setContent(html);
    ui->webView_5->setContent(html);
    ui->webView_6->setContent(html);

    m_threadParser = new Parser(user.name, user.password);

    connect(m_threadParser, SIGNAL(studentName(Parser::Student)), this, SLOT(jmeno(Parser::Student)));
    connect(m_threadParser, SIGNAL(done(QString, QByteArray)), this, SLOT(nacti(QString, QByteArray)));
    connect(m_threadParser, SIGNAL(error(Parser::Error)), this, SLOT(slotChyba(Parser::Error)));
    connect(m_threadParser, SIGNAL(finished()), this, SLOT(deleteThread()));

    m_threadParser->start();
}

void SuplChecker::deleteThread()
{
    Parser* thread = qobject_cast<Parser*>(sender());
    if (!thread) {
        return;
    }

    thread->deleteLater();
}

void SuplChecker::aboutProgram()
{
    if (!m_aboutDialog) {
        m_aboutDialog = new AboutDialog(this);
    }

    m_aboutDialog->show();
}

void SuplChecker::showSettingsDialog()
{
    SettingsDialog* window = new SettingsDialog(this, this);
    connect(window, SIGNAL(userModified(GlobalSettings::User, GlobalSettings::User)), this, SLOT(userModified(GlobalSettings::User, GlobalSettings::User)));
    window->show();
}

void SuplChecker::userModified(const GlobalSettings::User &before, const GlobalSettings::User &after)
{
    if (m_activeUser != before) {
        return;
    }

    m_activeUser = after;
}

void SuplChecker::nacti(QString info, QByteArray data)
{
    CHECK_THREAD;

    if (info == "tentotyden.html") {
        ui->webView_1->page()->mainFrame()->setContent(data);
    }
    else if (info == "dalsityden.html") {
        ui->webView_2->page()->mainFrame()->setContent(data);
    }
    else if (info == "stalyrozvrh.html") {
        ui->webView_3->page()->mainFrame()->setContent(data);
    }
    else if (info == "znamky.html") {
        ui->webView_4->page()->mainFrame()->setContent(data);
    }
    else if (info == "pololetni.html") {
        ui->webView_5->page()->mainFrame()->setContent(data);
    }
    else if (info == "planakci.html") {
        ui->webView_6->page()->mainFrame()->setContent(data);

        //Scroll to actual date
        QString actualMonth = "." + QString::number(QDate::currentDate().month()) + ".";
        QWebElementCollection col = ui->webView_6->page()->mainFrame()->documentElement().findAll("div[class=\"pldena\"]");
        foreach(QWebElement element, col) {
            if (element.toPlainText().contains(actualMonth)) {
                element.prependInside("<a name=\"scrollhere\"> </a>");
                break;
            }
        }

        ui->webView_6->page()->mainFrame()->scrollToAnchor("scrollhere");
    }
}

void SuplChecker::jmeno(const Parser::Student &s)
{
    CHECK_THREAD;

    if (s.jmeno.isEmpty() || s.trida.isEmpty()) {
        qCritical() << "received invalid student name!";
        return;
    }

    m_usersButton->setText(s.jmeno + " (" + s.trida + ")");

    GlobalSettings::User usr = m_activeUser;
    usr.realName = s.jmeno;

    GlobalSettings::AllUsers.removeOne(m_activeUser);
    GlobalSettings::AllUsers.append(usr);
    m_activeUser = usr;
}

void SuplChecker::aktualizace(QString nova, QString changelog)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Aktualizace!");
    msgBox.setText("<h2>Je dostupná aktualizace programu!</h2><b>Vaše verze: </b>" + SuplChecker::VERSION + "<br/><b>Nová verze: </b>" + nova + "<br/><br/>" + changelog + "<br/><br/>"
                   "<small><a href='http://suplchecker.wz.cz/'>Novou verzi stáhnete zde!</a></small>");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowIcon(QIcon(":icon.png"));
    msgBox.exec();
}

void SuplChecker::slotChyba(Parser::Error er)
{
    CHECK_THREAD;

    chyba(er);
}

void SuplChecker::chyba(Parser::Error er)
{
    QString errorString;
    QPixmap icon;

    switch (er) {
    case Parser::NoServersAvailable:
        icon = QPixmap(":/icons/alert.png");
        errorString = "Žádný z nastavených serverů není dostupný. Zkontrolujte Vaše internetové připojení O_o";
        break;

    case Parser::BadLogins:
        icon = QPixmap(":/icons/alert.png");
        errorString = "Zadané údaje jsou chybné :-!";
        break;

    case Parser::NoStartupUser:
        icon = QPixmap(":/icon.png");
        errorString = "Zadejte prosím Vaše údaje v Nastavení :-)";

    default:
        break;
    }

    m_usersButton->setText("Nevybrán uživatel");

    if (m_errorFrame) {
        delete m_errorFrame;
    }

    m_errorFrame = new QFrame(ui->tabWidget);
    m_errorFrame->setAttribute(Qt::WA_DeleteOnClose);
    m_errorFrame->setObjectName("fr");
    m_errorFrame->setStyleSheet("#fr{background: url(':icons/semi-transp.png');} QFrame[js-frame='true']{border: 1px solid  black;"
                                "border-bottom: 0px;background-color: white;}"
                                "QFrame[js-frame2='true']{border: 1px solid  black;border-top: 0px;background-color: #f3f3f3;}"
                               );

    Ui_ErrorOverlay* erUi = new Ui_ErrorOverlay();
    erUi->setupUi(m_errorFrame);
    erUi->message->setText(errorString);
    erUi->icon->setPixmap(icon);
    m_errorFrame->resize(ui->tabWidget->size());
    m_errorFrame->show();
    connect(erUi->buttonBox, SIGNAL(clicked(QAbstractButton*)), erUi->mainFrame, SLOT(close()));
}

void SuplChecker::reloadWithSameUser()
{
    if (GlobalSettings::AllUsers.contains(m_activeUser)) {
        startLoading(m_activeUser);
    }
    else {
        startLoading(GlobalSettings::AllUsers.at(0));
    }
}

void SuplChecker::aboutToShowUsersMenu()
{
    m_usersMenu->clear();

    foreach(GlobalSettings::User usr, GlobalSettings::AllUsers) {
        QString realName = usr.realName;
        if (realName.isEmpty()) {
            realName = usr.name;
        }

        m_usersMenu->addAction(QIcon(":icon.png"), "Načí­st uživatele " + realName, this, SLOT(loadAction()))->setData(usr.name);
    }
}

void SuplChecker::vybrano(QString text)
{
    foreach(GlobalSettings::User usr, GlobalSettings::AllUsers) {
        if (usr.name == text) {
            startLoading(usr);
            return;
        }
    }
}

void SuplChecker::loadAction()
{
    if (QAction* action = qobject_cast<QAction*>(sender())) {
        vybrano(action->data().toString());
    }
}

void SuplChecker::closeEvent(QCloseEvent* e)
{
    qApp->closeAllWindows();
    e->accept();
}

SuplChecker::~SuplChecker()
{
    GlobalSettings::saveSettings();
    delete ui;
}
