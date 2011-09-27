#include "webview.h"

WebPage::WebPage(QWidget *parent) :
    QWebPage(parent)
{
}

QString WebPage::userAgentForUrl(const QUrl &url) const
{
    Q_UNUSED(url)
    return "SuplChecker (http://suplchecker.wz.cz)";
}

WebView::WebView(QWidget *parent) :
    QWebView(parent)
{
    setPage(new WebPage(this));
}
