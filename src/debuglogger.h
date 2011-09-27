#ifndef DEBUGLOGGER_H
#define DEBUGLOGGER_H

#include <QWidget>

namespace Ui {
    class DebugLogger;
}

class DebugLogger : public QWidget
{
    Q_OBJECT

public:
    explicit DebugLogger(QWidget *parent = 0);
    ~DebugLogger();

    static void recordLog(const QString &log);

private slots:
    void clearLog();
    void loadLog();

private:
    static QString m_log;

    Ui::DebugLogger *ui;
};

#endif // DEBUGLOGGER_H
