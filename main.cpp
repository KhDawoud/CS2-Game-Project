#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "player.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QGraphicsScene *scene = new QGraphicsScene();
    Player *player = new Player();

    scene->addItem(player);

    QGraphicsView *view = new QGraphicsView(scene);
    view->setFixedSize(800, 600);
    view->show();

    return a.exec();
}