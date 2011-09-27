#include "loader.h"

Loader::Loader(QWidget* parent) :
    QLabel(parent)
{
    setFixedWidth(QPixmap(":icons/loader.gif").width());
}

void Loader::startAnimation()
{
    QMovie* movie = new QMovie(":icons/loader.gif");
    movie->setSpeed(70);
    setMovie(movie);
    movie->start();
}

void Loader::stopAnimation()
{
    movie()->stop();
    delete movie();
    clear();
}
