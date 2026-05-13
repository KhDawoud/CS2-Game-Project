#include "key.hpp"

#include "player.hpp"
#include <QGraphicsScene>
#include <cmath>

Key::Key(float x, float y, Player *p, QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent), player(p)
{

    spriteSheet.load(":resources/dungeon-assets/Key.png");
    shadowSpriteSheet.load(":resources/heart/heart_shadow.png");

    shadow = new QGraphicsPixmapItem();
    shadow->setPixmap(shadowSpriteSheet);

    this->setPixmap(spriteSheet);
    this->setPos(x, y);

    shadow->setPos(x-15, y + 30);

    this->setZValue(50);
    shadow->setZValue(49);

    this->setScale(0.7);
    this->setRotation(90);
    shadow->setScale(0.75);

    startY = y;
    bobTime = 0.0f;
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &Key::updateTick);
    updateTimer->start(16);
}

void Key::updateTick()
{
    if (this->scene() && shadow->scene() == nullptr)
    {
        this->scene()->addItem(shadow);
    }

    bobTime += 0.05f;
    float newY = startY + (std::sin(bobTime) * 4.0f);

    this->setPos(this->x(), newY);

    QRectF keyBox = this->sceneBoundingRect();
    QRectF playerBox = player->getPlayerHitbox(player->pos());

    if (keyBox.intersects(playerBox))
    {
        player->pickupkey();
        despawn();
    }
}


void Key::despawn()
{
    updateTimer->stop();
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

