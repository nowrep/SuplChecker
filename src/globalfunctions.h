#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QByteArray>
#include <QPixmap>
#include <QBuffer>
#include <QFile>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>

QByteArray sc_pixmapToByteArray(const QPixmap &pix, const char* type = "PNG");
QByteArray sc_readAllFileContents(const QString &filename);

void sc_centerWidgetOnScreen(QWidget* w);

#endif // GLOBALFUNCTIONS_H
