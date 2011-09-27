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
#include "suplchecker.h"
#include "parser.h"
#include "ui_mainwindow.h"
#include "nastaveni.h"
#include "globalsettings.h"
#include "loader.h"
#include "qtwin.h"
#include "aboutdialog.h"
#include "ui_erroroverlay.h"
#include "globalfunctions.h"

const QString SuplChecker::VERSION = "0.8.0";
const QString SuplChecker::BUILDTIME = __DATE__" "__TIME__;
const QString SuplChecker::AUTHOR = "nowrep";
const QString SuplChecker::COPYRIGHT = "2010-2011";
const QString SuplChecker::WWWADDRESS = "http://suplchecker.wz.cz";

SuplChecker::SuplChecker(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SuplChecker)
    , m_aboutDialog(0)
    , m_threadParser(0)
    , m_errorFrame(0)
    , m_updateShown(false)
    , m_isLoading(false)
{
    GlobalSettings::loadSettings();

    QTranslator* translator = new QTranslator();
    translator->load(GlobalSettings::DataDir + "qt_cs.qm");
    qApp->installTranslator(translator);

    ui->setupUi(this);
    QApplication::setWindowIcon(QIcon(":icons/icon.png"));

    if (QtWin::isCompositionEnabled()) {
        setContentsMargins(0, 0, 0, 0);
        QtWin::extendFrameIntoClientArea(this);
        setStyleSheet("QToolBar {border:none; background:transparent; }");
    }

    QToolBar* leftToolbar = new QToolBar(this);
    leftToolbar->setContextMenuPolicy(Qt::CustomContextMenu);
    QWidget* spa = new QWidget();
    spa->setMinimumWidth(170);
    m_loader = new Loader(leftToolbar);
    leftToolbar->addWidget(m_loader);
    leftToolbar->addWidget(spa);
    leftToolbar->addAction(QIcon(":/icons/reload.png"), "Obnovit", this, SLOT(reloadWithSameUser()))->setShortcut(QKeySequence("F5"));
    leftToolbar->addAction(QIcon(":/icons/book.png"), "Nastavení", this, SLOT(showSettingsDialog()))->setShortcut(QKeySequence("Ctrl+P"));
    leftToolbar->addAction(QIcon(":/icons/about.png"), "O programu", this, SLOT(aboutProgram()));

    m_usersButton = new QToolButton(this);
    m_usersButton->setText("Nevybrán uživatel");
    m_usersButton->setPopupMode(QToolButton::InstantPopup);
    m_usersMenu = new QMenu(this);
    m_usersButton->setMenu(m_usersMenu);

    leftToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    leftToolbar->setMovable(false);
    leftToolbar->setIconSize(QSize(20,20));
    addToolBar(leftToolbar);

    QToolBar* rightToolbar = new QToolBar(this);
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightToolbar->addWidget(spacer);
    rightToolbar->addWidget(m_usersButton);

    rightToolbar->setMovable(false);
    rightToolbar->setContextMenuPolicy(Qt::CustomContextMenu);
    addToolBar(rightToolbar);

    //Starting loading first user
    GlobalSettings::User startupUser = GlobalSettings::StartupUser;

    if (startupUser.name == "jmeno" || startupUser.password == "heslo" || GlobalSettings::AvailableServers.isEmpty())
        showSettingsDialog();
    else
        startLoading(startupUser);

    sc_centerWidgetOnScreen(this);

    connect(m_usersMenu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowUsersMenu()));
}

void SuplChecker::setLoading(bool set)
{
    m_isLoading = set;

    if (set)
        m_loader->startAnimation();
    else
        m_loader->stopAnimation();
}

void SuplChecker::startLoading(GlobalSettings::User user)
{
    if (m_isLoading || m_threadParser)
        return;

    if (m_errorFrame)
        delete m_errorFrame;

    m_usersButton->setText("Načítám ...");
    m_activeUser = user;

    QByteArray bgData = sc_pixmapToByteArray(QPixmap(GlobalSettings::BackgroundPixmapPath));
    QByteArray html = sc_readAllFileContents(":html/loading.html");
    html.replace("%BG-IMG%", bgData);
    html.replace("%PROGRESS%", sc_readAllFileContents(":html/progress.txt"));

    ui->webView_1->setContent(html);
    ui->webView_2->setContent(html);
    ui->webView_3->setContent(html);
    ui->webView_4->setContent(html);
    ui->webView_5->setContent(html);

    m_threadParser = new Parser(user.name, user.password, m_updateShown);
    m_threadParser->setBackgroundData(bgData);

    connect(m_threadParser, SIGNAL(studentName(Parser::Student)), this, SLOT(jmeno(Parser::Student)), Qt::QueuedConnection);
    connect(m_threadParser, SIGNAL(aktualizace(QString,QString,QString)), this, SLOT(aktualizace(QString,QString,QString)), Qt::QueuedConnection);
    connect(m_threadParser, SIGNAL(done(QString,QByteArray)), this, SLOT(nacti(QString,QByteArray)), Qt::QueuedConnection);
    connect(m_threadParser, SIGNAL(error(Parser::Error)), this, SLOT(chyba(Parser::Error)), Qt::QueuedConnection);
    connect(m_threadParser, SIGNAL(loading(bool)), this, SLOT(setLoading(bool)), Qt::QueuedConnection);
    connect(m_threadParser, SIGNAL(deleteNow()),this, SLOT(deleteThread()), Qt::QueuedConnection);

    m_threadParser->start();
}

void SuplChecker::deleteThread()
{
    delete m_threadParser;
}

void SuplChecker::aboutProgram()
{
    if (!m_aboutDialog)
        m_aboutDialog = new AboutDialog();

    m_aboutDialog->show();
    m_aboutDialog->raise();
}

void SuplChecker::showSettingsDialog()
{
    SettingsDialog window(this,this);
    connect(&window, SIGNAL(userModified(GlobalSettings::User,GlobalSettings::User)), this, SLOT(userModified(GlobalSettings::User,GlobalSettings::User)));
    window.exec();
}

void SuplChecker::userModified(const GlobalSettings::User &before, const GlobalSettings::User &after)
{
    if (m_activeUser != before)
        return;

    m_activeUser = after;
}

void SuplChecker::nacti(QString info, QByteArray data)
{
    if (info=="tentotyden.html")
        ui->webView_1->page()->mainFrame()->setContent(data);
    if (info=="dalsityden.html")
        ui->webView_2->page()->mainFrame()->setContent(data);
    if (info=="stalyrozvrh.html")
        ui->webView_3->page()->mainFrame()->setContent(data);
    if (info=="znamky.html")
        ui->webView_4->page()->mainFrame()->setContent(data);
    if (info=="pololetni.html")
        ui->webView_5->page()->mainFrame()->setContent(data);
}

void SuplChecker::jmeno(const Parser::Student &s)
{
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

void SuplChecker::aktualizace(QString stara, QString nova, QString changelog)
{
    if (m_updateShown == 0){
        m_updateShown = 1;
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Aktualizace!");
        msgBox.setText("<h2>Je dostupná aktualizace programu!</h2><b>Vaše verze: </b>"+stara+"<br/><b>Nová verze: </b>"+nova+"<br/><br/>"+changelog+"<br/><br/>"
                       "<small><a href='http://suplchecker.wz.cz/download.php'>Novou verzi stáhnete zde!</a></small>");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowIcon(QIcon(":icon.png"));
        msgBox.exec();
    }
}

void SuplChecker::chyba(Parser::Error er)
{
    QString errorString;

    switch (er) {
    case Parser::NoServersAvailable:
        errorString = "Žádný z nastavených serverů není dostupný. Zkontrolujte Vaše internetové připojení.";
        break;

    case Parser::BadLogins:
        errorString = "Zadané údaje jsou chybné.";
        break;

    default:
        break;
    }

    m_usersButton->setText("Nevybrán uživatel");

    if (m_errorFrame)
        delete m_errorFrame;

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
    m_errorFrame->resize(ui->tabWidget->size());
    m_errorFrame->show();
    connect(erUi->buttonBox, SIGNAL(clicked(QAbstractButton*)), erUi->mainFrame, SLOT(close()));
}

void SuplChecker::reloadWithSameUser()
{
    startLoading(m_activeUser);
}

void SuplChecker::aboutToShowUsersMenu()
{
    m_usersMenu->clear();

    foreach (GlobalSettings::User usr, GlobalSettings::AllUsers) {
        QString realName = usr.realName;
        if (realName.isEmpty())
            realName = usr.name;

        m_usersMenu->addAction(QIcon(":icon.png"), "Načí­st uživatele "+realName, this, SLOT(loadAction()))->setData(usr.name);
    }
}

void SuplChecker::vybrano(QString text)
{
    foreach (GlobalSettings::User usr, GlobalSettings::AllUsers) {
        if (usr.name == text) {
            startLoading(usr);
            return;
        }
    }
}

void SuplChecker::loadAction()
{
    if (QAction *action = qobject_cast<QAction*>(sender()))
        vybrano(action->data().toString());
}

void SuplChecker::closeEvent(QCloseEvent *e)
{
    qApp->closeAllWindows();
    e->accept();
}

SuplChecker::~SuplChecker()
{
    GlobalSettings::saveSettings();
    delete ui;
}
