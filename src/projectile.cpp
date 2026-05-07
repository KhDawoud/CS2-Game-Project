#include "projectile.hpp"
#include "characters.hpp"
#include "map2.hpp"
#include "house_interior-2.hpp"
#include "AudioManager.hpp"

Projectile::Projectile(QPointF startPos, QPointF direction, Map* Map1, House_Interior* Map2): gamemap(Map1), housemap(Map2) {
    sheet.load(":resources/player/Fireball.png");

    int frameWidth = sheet.width() / 5;
    int frameHeight = sheet.height();
    setPixmap(sheet.copy(0, 0, frameWidth, frameHeight));
    setOffset(-frameWidth / 2, -frameHeight / 2);

    velocity = direction;
    setPos(startPos);
    startPoint = startPos;

    setTransformOriginPoint(0, 0);

    if (velocity.x() > 0) setRotation(0);
    else if (velocity.x() < 0) setRotation(180);
    else if (velocity.y() < 0) setRotation(270);
    else if (velocity.y() > 0) setRotation(90);

    setPixmap(sheet.copy(0, 0, frameWidth, sheet.height()));
    this->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Projectile::move);
    timer->start(16);
}

void Projectile::move() {
    setPos(pos() + velocity * speed);
    currentFrame++;
    if (currentFrame >= maxFrames) currentFrame = 0;

    int frameWidth = sheet.width() / maxFrames;
    setPixmap(sheet.copy(currentFrame * frameWidth, 0, frameWidth, sheet.height()));

    if (!scene()) return;
    qreal BoxSize = 24;
    QRectF hitBox(x() - BoxSize/2, y() - BoxSize/2, BoxSize, BoxSize);

    if (gamemap) {
        const std::vector<PlacedObject>& obstacles = gamemap->getCollidableObjects();
        for (const auto& obj : obstacles) {
            if (hitBox.intersects(obj.worldHitbox)) {
                this->deleteLater();
                return;
            }
        }
    } else if (housemap) {
        const std::vector<HousePlacedObject>& obstacles = housemap->getCollidableObjects();
        for (const auto& obj : obstacles) {
            if (hitBox.intersects(obj.worldHitbox)) {
                this->deleteLater();
                return;
            }
        }
    }
    QList<QGraphicsItem *> hitItems = scene()->items(hitBox);

    for (QGraphicsItem *item : hitItems) {
        if (item == this || dynamic_cast<Characters*>(item)) continue;

        BaseEnemy *enemy = dynamic_cast<BaseEnemy *>(item);
        if (enemy && !enemy->isdead()) {
            enemy->TakeDamage(20);
            this->deleteLater();
            return;
        }
    }
    if (!scene()->sceneRect().contains(pos())) {
        this->deleteLater();
    }
    qreal distanceTraveled = QLineF(startPoint, pos()).length();

    if (distanceTraveled > maxRange * 0.8) {
        qreal scale = 1.0 - ((distanceTraveled - maxRange * 0.8) / (maxRange * 0.2));
        this->setScale(scale);
    }

    if (distanceTraveled >= maxRange) {
        this->deleteLater();
    }
}
