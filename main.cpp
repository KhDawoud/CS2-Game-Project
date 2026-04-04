#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "player.hpp"
#include "map.hpp"
#include "AudioManager.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AudioManager::instance();

    Map *scene = new Map();
    scene->setSceneRect(0, 0, 40 * 32, 35 * 32);

    Player *player = new Player();
    player->setPos(40 * 7, 35 * 28);
    scene->addItem(player);

    QGraphicsView *view = new QGraphicsView(scene);
    view->setAlignment(Qt::AlignCenter);

    view->scale(3.0, 3.0);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    view->centerOn(player);
    QObject::connect(player, &Player::positionChanged, view, [view](QGraphicsItem *p)
                     { view->centerOn(p); });
    view->showMaximized();
    return a.exec();
}
