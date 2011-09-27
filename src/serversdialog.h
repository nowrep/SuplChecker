#ifndef SERVERSDIALOG_H
#define SERVERSDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
    class ServersDialog;
}

class ServersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServersDialog(QWidget *parent = 0);
    ~ServersDialog();

private slots:
    void itemUp();
    void itemDown();
    void itemDelete();
    void itemNew();

    void saveSettings();

private:
    Ui::ServersDialog *ui;
};

#endif // SERVERSDIALOG_H
