#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <slime.hpp>
#include "AudioManager.hpp"
#include "deathwindow.hpp"
#include "gameview.hpp"
#include "maploader.hpp"
#include "characterstats.hpp"
#include "characters.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AudioManager::instance();

    Characters *player = new Characters(1);

    CharacterStats *stats = new CharacterStats();
    stats->setPlayer(player);
    stats->setZValue(1000);

    QObject::connect(player, &Player::statsChanged, stats, &CharacterStats::updateBars);

    // every map is now an instance of maploader
    MapLoader *interior = new MapLoader(":/resources/map-data/house.json", player);
    interior->setSceneRect(0, 0, interior->mapCols() * interior->tileSize(),
                           interior->mapRows() * interior->tileSize());

    MapLoader *overworld = new MapLoader(":/resources/map-data/level-1.json", player);

    overworld->setSceneRect(0, 0, overworld->mapCols() * overworld->tileSize(),
                            overworld->mapRows() * overworld->tileSize());

    overworld->addItem(stats);

    GameView *view = new GameView(overworld, interior, player);

    QObject::connect(player,
                     &Player::positionChanged,
                     view,
                     [view, stats](QGraphicsItem *p)
                     {
                         view->centerOn(p);
                         stats->setPos(view->mapToScene(10, 10));
                     });

    stats->setPos(view->mapToScene(10, 10));

    view->setFocus();
    player->setFocus();

    QObject::connect(overworld,
                     &QGraphicsScene::focusItemChanged,
                     [player](QGraphicsItem *newFocus,
                              QGraphicsItem *,
                              Qt::FocusReason)
                     {
                         if (newFocus != player)
                             player->setFocus();
                     });
    QObject::connect(interior,
                     &QGraphicsScene::focusItemChanged,
                     [player](QGraphicsItem *newFocus,
                              QGraphicsItem *,
                              Qt::FocusReason)
                     {
                         if (newFocus != player)
                             player->setFocus();
                     });

    QObject::connect(player, &Player::playerDied, [view]()
                     {
        DeathWindow *deathScreen = new DeathWindow(view);
        deathScreen->exec(); });

    return a.exec();
}