#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <slime.hpp>
#include "AudioManager.hpp"
#include "deathwindow.hpp"
#include "gameview.hpp"
#include "house_interior-2.hpp"
#include "map2.hpp"
#include "characterstats.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AudioManager::instance();

    Player* player = new Player();

    CharacterStats* stats = new CharacterStats();
    stats->setPlayer(player);
    stats->setZValue(1000);

    QObject::connect(player, &Player::statsChanged, stats, &CharacterStats::updateBars);
    Map *scene = new Map(player);
    scene->setSceneRect(0, 0, 40 * 32, 35 * 32);
    scene->addItem(stats);

    House_Interior *interior = new House_Interior(player);
    interior->setSceneRect(0, 0, 16 * 32, 10 * 32);
    GameView *view = new GameView(scene, interior, player);
    QObject::connect(player,
                     &Player::positionChanged,
                     view,
                     [view, stats](QGraphicsItem *p) {
                         view->centerOn(p);
                         stats->setPos(view->mapToScene(10, 10));
                     });
    stats->setPos(view->mapToScene(10, 10));
    view->setFocus();
    player->setFocus();
    QObject::connect(scene,
                     &QGraphicsScene::focusItemChanged,
                     [player](QGraphicsItem *newFocus,
                                                   QGraphicsItem *oldFocus,
                                                   Qt::FocusReason reason) {
                         if (newFocus != player) {
                             player->setFocus();
                         }
                     });
    QObject::connect(player, &Player::playerDied, [view]() {
        DeathWindow *deathScreen = new DeathWindow(view);
        deathScreen->exec();
    });
    return a.exec();
}
