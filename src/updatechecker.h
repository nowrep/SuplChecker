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
    explicit UpdateChecker(QObject* parent = 0);

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
