// ----------------------------------------------------------------------------
// Trap sandbox.
// Temporary test main. To use it: open CMakeLists.txt and swap "main.cpp"
// for "main_traptest.cpp" in qt_add_executable(...). Build and run.
// Move with WASD/arrows. Watch the HP bar. Each trap is labelled.
// ----------------------------------------------------------------------------
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QFont>
#include <QPainter>

#include "characters.hpp"
#include "characterstats.hpp"
#include "traps.hpp"

static QGraphicsTextItem *makeLabel(const QString &text, qreal x, qreal y)
{
    auto *t = new QGraphicsTextItem(text);
    t->setDefaultTextColor(Qt::white);
    QFont f; f.setPointSize(10); f.setBold(true);
    t->setFont(f);
    t->setPos(x, y);
    t->setZValue(500);
    return t;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto *scene = new QGraphicsScene;
    scene->setBackgroundBrush(Qt::black);
    scene->setSceneRect(0, 0, 1200, 720);

    // --- player ---
    Characters *player = new Characters(1);
    player->setPos(60, 360);
    scene->addItem(player);
    player->setFocus();

    // HP overlay
    auto *stats = new CharacterStats;
    stats->setPlayer(player);
    stats->setZValue(1000);
    stats->setPos(10, 10);
    scene->addItem(stats);
    QObject::connect(player, &Player::statsChanged, stats, &CharacterStats::updateBars);

    // --- traps ---
    auto *spike  = new SpikeTrap();
    spike->setPos(180, 340);
    spike->setDamage(20);
    spike->setDamageCooldown(500);

    auto *slider = new SlidingSpikeTrap();
    slider->setPos(320, 340);
    slider->setDamage(15);
    slider->setDamageCooldown(600);

    auto *saw = new SawBladeTrap();
    saw->setPos(620, 340);
    saw->setDamage(25);
    saw->setDamageCooldown(400);

    auto *fire  = new FireTrap();
    fire->setPos(880, 340);

    auto *fire2 = new FireTrap2();
    fire2->setPos(880, 340 + fire->getheight() + 10);

    auto *fire3 = new FireTrap3();
    fire3->setPos(880, 340 + fire->getheight() + 10 + fire2->getheight() + 10);

    scene->addItem(spike);
    scene->addItem(slider);
    scene->addItem(saw);
    scene->addItem(fire);
    scene->addItem(fire2);
    scene->addItem(fire3);

    // Labels so you know which trap is which
    scene->addItem(makeLabel("SpikeTrap\n(animated)",                 150, 470));
    scene->addItem(makeLabel("SlidingSpikeTrap\n(animated in place)", 290, 480));
    scene->addItem(makeLabel("SawBladeTrap\n(spins in place)",        570, 470));
    scene->addItem(makeLabel("FireTrap1",                             860, 470));
    scene->addItem(makeLabel("FireTrap2",                             990, 470));
    scene->addItem(makeLabel("FireTrap3",                             860, 540));

    // --- view ---
    auto *view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing, false);
    view->setFixedSize(1220, 760);
    view->show();

    return app.exec();
}
