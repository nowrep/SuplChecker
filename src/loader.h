#ifndef LOADER_H
#define LOADER_H

#include <QLabel>
#include <QMovie>

class Loader : public QLabel
{
    Q_OBJECT
public:
    explicit Loader(QWidget* parent = 0);
    void startAnimation();
    void stopAnimation();

signals:

public slots:

};

#endif // LOADER_H
