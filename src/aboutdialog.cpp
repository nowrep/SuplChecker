#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "debuglogger.h"
#include "suplchecker.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AboutDialog)
    , m_debugLogger(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() ^ Qt::WindowMaximizeButtonHint ^ Qt::WindowMinimizeButtonHint);

    ui->setupUi(this);
    SuplChecker::centerOnScreen(this);

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(close()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(openLog()));
}

void AboutDialog::openLog()
{
    if (!m_debugLogger)
        m_debugLogger = new DebugLogger();

    m_debugLogger->show();
    m_debugLogger->raise();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
