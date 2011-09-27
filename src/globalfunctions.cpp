#include "globalfunctions.h"

QByteArray sc_pixmapToByteArray(const QPixmap &pix, const char *type)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    if (pix.save(&buffer, type))
        return buffer.buffer().toBase64();

    return QByteArray();
}

QByteArray sc_readAllFileContents(const QString &filename)
{
    QFile file(filename);
    file.open(QFile::ReadOnly);
    QByteArray a = file.readAll();
    file.close();

    return a;
}

void sc_centerWidgetOnScreen(QWidget *w)
{
    const QRect screen = QApplication::desktop()->screenGeometry();
    const QRect &size = w->geometry();
    w->move( (screen.width()-size.width())/2, (screen.height()-size.height())/2 );
}
