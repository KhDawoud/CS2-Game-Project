#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "player.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QGraphicsScene *scene = new QGraphicsScene();
    // the reason for the weird slowdown at the start was this wasn't set.
    // when u start, it was unaware of the size of the scene so just drew a box around you and once
    // u pass it it extends the box, causing the slowdown.
    scene->setSceneRect(0, 0, 800, 600);
    Player *player = new Player();

    scene->addItem(player);

    QGraphicsView *view = new QGraphicsView(scene);
    view->setFixedSize(800, 600);
    view->show();

    return a.exec();
}