#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QFont>
#include <QPainter>
#include <QTimer>
#include <slime.hpp>
#include "AudioManager.hpp"
#include "deathwindow.hpp"
#include "gameview.hpp"
#include "maploader.hpp"
#include "characterstats.hpp"
#include "characters.hpp"
#include "traps.hpp"

static QGraphicsTextItem *makeLabel(const QString &text, qreal x, qreal y)
{
    auto *t = new QGraphicsTextItem(text);
    t->setDefaultTextColor(Qt::white);
    QFont f;
    f.setPointSize(10);
    f.setBold(true);
    t->setFont(f);
    t->setPos(x, y);
    t->setZValue(500);
    return t;
}

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
    MapLoader *interior = new MapLoader(":resources/map-data/house.json", player);
    interior->setSceneRect(0, 0, interior->mapCols() * interior->tileSize(),
                           interior->mapRows() * interior->tileSize());

    MapLoader *overworld = new MapLoader(":resources/map-data/test-level-2.json", player);

    overworld->setSceneRect(0, 0, overworld->mapCols() * overworld->tileSize(),
                            overworld->mapRows() * overworld->tileSize());

    overworld->addItem(stats);

    QObject::connect(overworld, &MapLoader::levelCleared, player, &Characters::handleLevelCleared);

    GameView *view = new GameView(overworld, interior, player);

    QObject::connect(player,
                     &Player::positionChanged,
                     view,
                     [view, stats](QGraphicsItem *p)
                     {
                         view->centerOn(p);
                         stats->setPos(view->mapToScene(10, 10));
                         view->checkInteractions();
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
