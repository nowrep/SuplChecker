#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>

class WebPage : public QWebPage
{
    Q_OBJECT
public:
    explicit WebPage(QWidget *parent = 0);
    QString userAgentForUrl(const QUrl &url) const;
};

class WebView : public QWebView
{
    Q_OBJECT
public:
    explicit WebView(QWidget *parent = 0);

signals:

public slots:

};

#endif // WEBVIEW_H
