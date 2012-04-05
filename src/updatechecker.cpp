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
#include "updatechecker.h"
#include "suplchecker.h"

UpdateChecker::UpdateChecker(QObject* parent)
    : QThread(parent)
    , m_manager(new QNetworkAccessManager)
{
}

void UpdateChecker::run()
{
    QTimer::singleShot(0, this, SLOT(startWork()));
    exec();
}

QString UpdateChecker::send_request(const QUrl &url, Operation method, QByteArray postData)
{
    Q_ASSERT(m_manager);

    QNetworkReply* reply;
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "SuplChecker - UpdateChecker");

    if (method == GET) {
        reply = m_manager->get(req);
    }
    else if (method == POST) {
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        reply = m_manager->post(req, postData);
    }

    QEventLoop eLoop;
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), &eLoop, SLOT(quit()));
    eLoop.exec();

    return reply->readAll();
}

void UpdateChecker::startWork()
{
    qDebug() << "checking for updates...";
    QString actual_version = send_request(QUrl("http://suplchecker.wz.cz/version.php"));
    qDebug() << "current version: " << SuplChecker::VERSION << " - actual version: " << actual_version;

    if (!actual_version.contains(SuplChecker::VERSION) && !actual_version.isEmpty()) {
        QString changelog = send_request(QUrl("http://suplchecker.wz.cz/changelog.php"));
        emit updateAvailable(actual_version, changelog);
    }
}
