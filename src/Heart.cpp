#include "Heart.hpp"
#include "player.hpp"
#include <QGraphicsScene>
#include <cmath>

Heart::Heart(float x, float y, Player *p, QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent), player(p)
{
    healAmount = 20;

    spriteSheet.load(":resources/heart/heal_heart2.png");
    this->setPixmap(spriteSheet);

    this->setPos(x, y);
    this->setZValue(50);
    this->setScale(0.30);

    // the spritesheet didn't really fit so I did this bobbing animation
    startY = y;
    bobTime = 0.0f;
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &Heart::updateTick);
    updateTimer->start(16);

    // timer to despawn if not picked up after a while
    lifespanTimer = new QTimer(this);
    lifespanTimer->setSingleShot(true);
    connect(lifespanTimer, &QTimer::timeout, this, &Heart::despawn);
    lifespanTimer->start(6000);
}

Heart::~Heart()
{
    // Qt cleans up children QObjects automatically
}

void Heart::updateTick()
{
    // this just does some math so the animation is smooth not choppy
    bobTime += 0.05f;
    float newY = startY + (std::sin(bobTime) * 5.0f);
    this->setPos(this->x(), newY);

    QRectF heartBox = this->sceneBoundingRect();
    QRectF playerBox = player->getPlayerHitbox(player->pos());

    if (heartBox.intersects(playerBox))
    {
        player->Heal(healAmount);
        despawn();
    }
}

void Heart::despawn()
{
    updateTimer->stop();
    lifespanTimer->stop();

    if (scene())
    {
        scene()->removeItem(this);
    }
    this->deleteLater();
}