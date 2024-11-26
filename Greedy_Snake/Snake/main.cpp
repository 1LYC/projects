#include "gamehall.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameHall w;
    w.show();
    return a.exec();
}
