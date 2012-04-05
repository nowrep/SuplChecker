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
#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>

class WebPage : public QWebPage
{
    Q_OBJECT
public:
    explicit WebPage(QWidget* parent = 0);
    QString userAgentForUrl(const QUrl &url) const;
};

class WebView : public QWebView
{
    Q_OBJECT
public:
    explicit WebView(QWidget* parent = 0);

signals:

public slots:

};

#endif // WEBVIEW_H
