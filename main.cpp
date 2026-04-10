#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <slime.hpp>
// mish lazem /include since it's defined in the cmake
#include "AudioManager.hpp"
#include "deathwindow.hpp"
#include "gameview.hpp"
#include "map2.hpp"
#include "house_interior.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AudioManager::instance();

    Map *scene = new Map();
    scene->setSceneRect(0, 0, 40 * 32, 35 * 32);

    House_Interior *interior = new House_Interior();
    interior->setSceneRect(0, 0, 16 * 32, 10 * 32);

    GameView *view = new GameView(scene, interior, scene->getPlayer());

    QObject::connect(scene->getPlayer(), &Player::positionChanged, view, [view, stats = scene->getStats()](QGraphicsItem *p)
                     {
        view->centerOn(p);
        //draws the statbar in the top left corner regardless of window size and position
        stats->setPos(view->mapToScene(10, 10)); });
    scene->getStats()->setPos(view->mapToScene(10, 10));

    view->setFocus();
    scene->getPlayer()->setFocus();
    // making sure the glitch where player goes out of focus doesn't happen
    QObject::connect(scene,
                     &QGraphicsScene::focusItemChanged,
                     [player = scene->getPlayer()](QGraphicsItem *newFocus,
                                                   QGraphicsItem *oldFocus,
                                                   Qt::FocusReason reason)
                     {
                         if (newFocus != player)
                         {
                             player->setFocus();
                         }
                     });

    QObject::connect(scene->getPlayer(), &Player::playerDied, [view]()
                     {
        DeathWindow *deathScreen = new DeathWindow(view);
        deathScreen->exec(); });

    return a.exec();
}
