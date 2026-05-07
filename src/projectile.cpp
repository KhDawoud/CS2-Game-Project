#include "projectile.hpp"
#include "characters.hpp"
#include "map2.hpp"
#include "house_interior-2.hpp"
#include "AudioManager.hpp"
#include <cmath>

Projectile::Projectile(QPointF startPos, QPointF direction, MapLoader *map) : currentMap(map)
{
    sheet.load(":resources/player/Fireball.png");
    explosionSheet.load(":/resources/player/Circle_explosion1-sheet.png");
    int frameWidth = sheet.width() / 5;
    int frameHeight = sheet.height();
    setPixmap(sheet.copy(0, 0, frameWidth, frameHeight));
    setOffset(-frameWidth / 2, -frameHeight / 2);

    velocity = direction;
    setPos(startPos);
    startPoint = startPos;

    setTransformOriginPoint(0, 0);

    if (velocity.x() > 0)
        setRotation(0);
    else if (velocity.x() < 0)
        setRotation(180);
    else if (velocity.y() < 0)
        setRotation(270);
    else if (velocity.y() > 0)
        setRotation(90);

    setPixmap(sheet.copy(0, 0, frameWidth, sheet.height()));
    this->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Projectile::move);
    timer->start(16);
}

void Projectile::move()
{
    if (isExploding)
    {
        updateAnimation();
        return;
    }
    setPos(pos() + velocity * speed);
    bool hitSomething = false;

    currentFrame++;
    if (currentFrame >= maxFrames)
        currentFrame = 0;

    int frameWidth = sheet.width() / maxFrames;
    setPixmap(sheet.copy(currentFrame * frameWidth, 0, frameWidth, sheet.height()));

    if (!scene())
        return;
    qreal BoxSize = 24;
    QRectF hitBox(x() - BoxSize / 2, y() - BoxSize / 2, BoxSize, BoxSize);

    if (currentMap)
    {
        float tileSize = static_cast<float>(currentMap->tileSize());
        int leftCol = static_cast<int>(std::floor(hitBox.left() / tileSize));
        int rightCol = static_cast<int>(std::floor(hitBox.right() / tileSize));
        int topRow = static_cast<int>(std::floor(hitBox.top() / tileSize));
        int bottomRow = static_cast<int>(std::floor(hitBox.bottom() / tileSize));

        for (int r = topRow; r <= bottomRow; ++r)
        {
            for (int c = leftCol; c <= rightCol; ++c)
            {
                if (currentMap->isTileCollidable(r, c))
                {
                    hitSomething = true;
                    break;
                }
            }
            if (hitSomething)
                break;
        }

        if (!hitSomething)
        {
            const auto &objects = currentMap->getActiveCollidables();
            for (const auto &obj : objects)
            {
                if (hitBox.intersects(obj.worldHitbox))
                {
                    hitSomething = true;
                    break;
                }
            }
        }
    }
    if (!hitSomething)
    {
        QList<QGraphicsItem *> hitItems = scene()->items(hitBox);
        for (QGraphicsItem *item : hitItems)
        {
            if (item == this || dynamic_cast<Characters *>(item))
                continue;

            BaseEnemy *enemy = dynamic_cast<BaseEnemy *>(item);
            if (enemy && !enemy->isdead())
            {
                hitSomething = true;
                enemy->TakeDamage(20);
                break;
            }
        }
    }
    if (!scene()->sceneRect().contains(pos()))
    {
        this->deleteLater();
    }
    qreal distanceTraveled = QLineF(startPoint, pos()).length();

    if (distanceTraveled > maxRange * 0.8)
    {
        qreal scale = 1.0 - ((distanceTraveled - maxRange * 0.8) / (maxRange * 0.2));
        this->setScale(scale);
    }
    if (hitSomething)
    {
        startExplosion();
        return;
    }

    if (distanceTraveled >= maxRange)
    {
        this->deleteLater();
    }
}
void Projectile::startExplosion()
{
    isExploding = true;
    explosionFrame = 0;
    explosionCounter = 0;

    setRotation(0);
    setScale(0.5);

    int fw = explosionSheet.width() / 10;
    setPixmap(explosionSheet.copy(0, 0, fw, explosionSheet.height()));
    setOffset(-fw / 2, -explosionSheet.height() / 2);
}
void Projectile::updateAnimation()
{
    explosionCounter++;

    if (explosionCounter < 4)
    {
        return;
    }

    explosionCounter = 0;
    explosionFrame++;

    if (explosionFrame >= 10)
    {
        this->deleteLater();
        return;
    }

    int fw = explosionSheet.width() / 10;
    int fh = explosionSheet.height();

    setPixmap(explosionSheet.copy(explosionFrame * fw, 0, fw, fh));
    setOffset(-fw / 2, -fh / 2);
}
