#include "debuglogger.h"
#include "ui_debuglogger.h"
#include "mainwindow.h"

QString DebugLogger::m_log;

DebugLogger::DebugLogger(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DebugLogger)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() ^ Qt::WindowMaximizeButtonHint ^ Qt::WindowMinimizeButtonHint);

    ui->setupUi(this);
    SuplChecker::centerOnScreen(this);

    loadLog();

    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearLog()));
    connect(ui->obnovit, SIGNAL(clicked()), this, SLOT(loadLog()));

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(close()));
}

void DebugLogger::loadLog()
{
    ui->textBrowser->setText(m_log);
}

void DebugLogger::clearLog()
{
    m_log.clear();
    ui->textBrowser->clear();
}

void DebugLogger::recordLog(const QString &log)
{
    if (log.isEmpty())
        return;

    m_log.prepend(log + "\n");
}

DebugLogger::~DebugLogger()
{
    delete ui;
}
