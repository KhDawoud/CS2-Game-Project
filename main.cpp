#include <QApplication>
#include <QCoreApplication>
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
#include "levelselectwindow.hpp"
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

    // Set these BEFORE any code that reads/writes the save file, so
    // QStandardPaths::AppDataLocation resolves to a stable path that doesn't
    // change between runs (otherwise "Continue" can silently lose progress).
    QCoreApplication::setOrganizationName("CS2GameProject");
    QCoreApplication::setApplicationName("CS2GameProject");

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

    MapLoader *overworld = new MapLoader(":resources/map-data/level-1.json", player);

    overworld->setSceneRect(0, 0, overworld->mapCols() * overworld->tileSize(),
                            overworld->mapRows() * overworld->tileSize());

    overworld->addItem(stats);

    MapLoader *level2 = new MapLoader(":resources/map-data/Level2.json", player);
    level2->setSceneRect(0, 0, level2->mapCols() * level2->tileSize(),
                         level2->mapRows() * level2->tileSize());

    MapLoader *level3 = new MapLoader(":resources/map-data/Level3.json", player);
    level3->setSceneRect(0, 0, level3->mapCols() * level3->tileSize(),
                         level3->mapRows() * level3->tileSize());

    QObject::connect(overworld, &MapLoader::levelCleared, player, &Characters::handleLevelCleared);
    QObject::connect(level2,   &MapLoader::levelCleared, player, &Characters::handleLevelCleared);
    QObject::connect(level3,   &MapLoader::levelCleared, player, &Characters::handleLevelCleared);

    GameView *view = new GameView(overworld, interior, level2, level3, player);
    view->restoreContinueState();

    QTimer *uiTimer = new QTimer();

    // we constantly move stats bar to the current level and load progress and boss bars
    QObject::connect(uiTimer, &QTimer::timeout, [view, stats, interior, level2, level3]()
                     {
        if (stats->scene() != view->scene() && view->scene() != nullptr && view->scene() != interior) {
            if (stats->scene()) {
                stats->scene()->removeItem(stats);
            }
            view->scene()->addItem(stats);
        }
        stats->setPos(view->mapToScene(10, 10));
    if (view->scene() == level3) {
            if (view->_progressBar) view->_progressBar->setVisible(false);
            if (view->_bossHealthBar) view->_bossHealthBar->setVisible(true);
            if (view->_bossLabel) view->_bossLabel->setVisible(true);
        } else {
            if (view->_progressBar) view->_progressBar->setVisible(true);
            if (view->_bossHealthBar) view->_bossHealthBar->setVisible(false);
            if (view->_bossLabel) view->_bossLabel->setVisible(false);
        } });

    uiTimer->start(16);

    QObject::connect(player,
                     &Player::positionChanged,
                     view,
                     [view, stats](QGraphicsItem *p)
                     {
                         view->centerOn(p);
                         view->viewport()->update();
                         stats->setPos(view->mapToScene(10, 10));
                         view->checkInteractions();
                     });

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
    QObject::connect(level2,
                     &QGraphicsScene::focusItemChanged,
                     [player](QGraphicsItem *newFocus,
                              QGraphicsItem *,
                              Qt::FocusReason)
                     {
                         if (newFocus != player)
                             player->setFocus();
                     });
    QObject::connect(level3,
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
        // Don't auto-continue into a dead save on next launch
        LevelSelectWindow::clearContinueState();
        DeathWindow *deathScreen = new DeathWindow(view);
        deathScreen->exec(); });

    return a.exec();
}
