#include "game.h"
#include <QApplication>

namespace Temp{
    Game* g;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Temp::g = new Game();
    Temp::g->show();

    return a.exec();
}
