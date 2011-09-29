#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QThread>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QEventLoop>

class UpdateChecker : public QThread
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = 0);

    enum Operation { GET, POST };

protected:
   void run();

signals:
   void updateAvailable(const QString &version, const QString &changelog);

public slots:

private:
   QNetworkAccessManager* m_manager;
   QString send_request(const QUrl &url, Operation method = GET, QByteArray postData = "");

private slots:
   void startWork();

};

#endif // UPDATECHECKER_H
