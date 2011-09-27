#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QWidget>
#include <QPointer>

namespace Ui {
    class AboutDialog;
}

class DebugLogger;
class AboutDialog : public QWidget
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private slots:
    void openLog();

private:
    Ui::AboutDialog *ui;
    QPointer<DebugLogger> m_debugLogger;
};

#endif // ABOUTDIALOG_H
