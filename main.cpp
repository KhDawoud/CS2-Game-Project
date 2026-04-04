#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "player.hpp"
#include "map.hpp"
#include "AudioManager.hpp"
#include "slime.hpp"
#include "characterstats.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AudioManager::instance();

    Map *scene = new Map();
    scene->setSceneRect(0, 0, 40 * 32, 35 * 32);

    Player *player = new Player();
    player->setPos(40 * 7, 35 * 28);
    scene->addItem(player);

    CharacterStats *stats = new CharacterStats();
    stats->setPlayer(player);
    stats->setZValue(100);
    scene->addItem(stats);
    QObject::connect(player, &Player::statsChanged, stats, &CharacterStats::updateBars);

    Slime *slime = new Slime();
    slime->setPos(40 * 22, 35 * 20);
    slime->setPlayer(player);
    scene->addItem(slime);

    QGraphicsView *view = new QGraphicsView(scene);
    view->setAlignment(Qt::AlignCenter);

    view->scale(3.0, 3.0);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    view->centerOn(player);
    QObject::connect(player, &Player::positionChanged, view, [view, stats](QGraphicsItem *p)
                     { 
                         view->centerOn(p); 
                         //draws the statbar in the top left corner regardless of window size and position
                         stats->setPos(view->mapToScene(10, 10)); });
    view->showMaximized();
    stats->setPos(view->mapToScene(10, 10));
    return a.exec();
}
