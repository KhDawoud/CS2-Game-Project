#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "include/AudioManager.hpp"
#include "include/characterstats.hpp"
#include "include/map.hpp"
#include "include/player.hpp"
#include "include/slime.hpp"
#include "include/gameview.hpp"

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

    GameView *view = new GameView(scene, player);

    QObject::connect(player, &Player::positionChanged, view, [view, stats](QGraphicsItem *p)
                     {
        view->centerOn(p);
        //draws the statbar in the top left corner regardless of window size and position
        stats->setPos(view->mapToScene(10, 10)); });
    stats->setPos(view->mapToScene(10, 10));

    view->setFocus();
    player->setFocus();

    // making sure the glitch where player goes out of focus doesn't happen
    QObject::connect(scene, &QGraphicsScene::focusItemChanged, [player](QGraphicsItem *newFocus, QGraphicsItem *oldFocus, Qt::FocusReason reason)
                     {
        if (newFocus != player) {
            player->setFocus();
        } });

    return a.exec();
}
