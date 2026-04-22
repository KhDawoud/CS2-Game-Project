#include "Heart.hpp"
#include "player.hpp"
#include <QGraphicsScene>
#include <cmath>

Heart::Heart(float x, float y, Player *p, QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent), player(p)
{
    healAmount = 20;

    spriteSheet.load(":resources/heart/heal_heart2.png");
    shadowSpriteSheet.load(":resources/heart/heart_shadow.png");

    shadow = new QGraphicsPixmapItem();
    shadow->setPixmap(shadowSpriteSheet);

    this->setPixmap(spriteSheet);
    this->setPos(x, y);

    shadow->setPos(x - 2.5, y + 10);

    this->setZValue(50);
    shadow->setZValue(49);

    this->setScale(0.30);
    shadow->setScale(0.75);

    startY = y;
    bobTime = 0.0f;
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &Heart::updateTick);
    updateTimer->start(16);

    lifespanTimer = new QTimer(this);
    lifespanTimer->setSingleShot(true);
    connect(lifespanTimer, &QTimer::timeout, this, &Heart::despawn);
    lifespanTimer->start(6000);
}

void Heart::updateTick()
{
    // makes sure shadow added after heart is
    if (this->scene() && shadow->scene() == nullptr)
    {
        this->scene()->addItem(shadow);
    }

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

Heart::~Heart()
{
    // Qt cleans up children QObjects automatically
}

void Heart::despawn()
{
    updateTimer->stop();
    lifespanTimer->stop();

    if (shadow)
    {
        if (shadow->scene())
        {
            shadow->scene()->removeItem(shadow);
        }
        delete shadow;
        shadow = nullptr;
    }

    if (scene())
    {
        scene()->removeItem(this);
    }
    this->deleteLater();
}