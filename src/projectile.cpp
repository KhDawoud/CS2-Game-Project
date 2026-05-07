#include "projectile.hpp"
#include "characters.hpp"
#include "map2.hpp"
#include "house_interior-2.hpp"
#include "AudioManager.hpp"

Projectile::Projectile(QPointF startPos, QPointF direction, Map* Map1, House_Interior* Map2): gamemap(Map1), housemap(Map2) {
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
    if (isExploding) {
        updateAnimation();
        return;
    }
    setPos(pos() + velocity * speed);
    bool hitSomething = false;

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
                hitSomething = true;
                break;
            }
        }
    } else if (housemap) {
        const std::vector<HousePlacedObject>& obstacles = housemap->getCollidableObjects();
        for (const auto& obj : obstacles) {
            if (hitBox.intersects(obj.worldHitbox)) {
                hitSomething = true;
                break;
            }
        }
    }
    if (!hitSomething) {
        QList<QGraphicsItem *> hitItems = scene()->items(hitBox);
        for (QGraphicsItem *item : hitItems) {
            if (item == this || dynamic_cast<Characters*>(item)) continue;

            BaseEnemy *enemy = dynamic_cast<BaseEnemy *>(item);
            if (enemy && !enemy->isdead()) {
                hitSomething = true;
                enemy->TakeDamage(20);
                break;
            }
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
    if (hitSomething) {
        startExplosion();
        return;
    }

    if (distanceTraveled >= maxRange) {
        this->deleteLater();
    }
}
void Projectile::startExplosion() {
    isExploding = true;
    explosionFrame = 0;
    explosionCounter = 0;

    setRotation(0); // Ensure the explosion faces "up"
    setScale(0.5);  // Optional: make the explosion bigger than the fireball

    // Set the first frame immediately
    int fw = explosionSheet.width() / 10;
    setPixmap(explosionSheet.copy(0, 0, fw, explosionSheet.height()));
    setOffset(-fw / 2, -explosionSheet.height() / 2);
}
void Projectile::updateAnimation() {
    explosionCounter++;

    if (explosionCounter < 4) {
        return;
    }


    explosionCounter = 0;
    explosionFrame++;

    if (explosionFrame >= 10) {
        this->deleteLater(); // Finally remove from scene after last frame
        return;
    }

    int fw = explosionSheet.width() / 10;
    int fh = explosionSheet.height();

    // Shift the "window" of the copy function to the next frame
    setPixmap(explosionSheet.copy(explosionFrame * fw, 0, fw, fh));
    setOffset(-fw / 2, -fh / 2);
}
