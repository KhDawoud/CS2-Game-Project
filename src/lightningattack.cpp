#include "lightningattack.hpp"
#include "Enemy.hpp"
#include <QTimer>
#include <cmath>
#include <characters.hpp>
#include "AudioManager.hpp"

LightningAttack::LightningAttack(QGraphicsItem* parent, MapLoader* Map)
    : QGraphicsPixmapItem(parent), currentMap(Map)
{
    spriteSheet = QPixmap(":resources/player/Lightning-sheet.png");
    if (spriteSheet.isNull()) {
        qDebug() << "Error: Lightning spritesheet not found!";
    } else {
        frameWidth = spriteSheet.width() / 14;
        frameHeight = spriteSheet.height();
    }
    this->setScale(0.5);
    this->setTransformOriginPoint(0, 0);

    setZValue(100);
    this->hide();

    QTimer* animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &LightningAttack::updateAnimation);
    animTimer->start(80);
}

void LightningAttack::updateAnimation() {
    if (state == BoltState::Inactive) return;

    int xOffset = 0;
    switch (state) {
    case BoltState::Starting:
        xOffset = currentFrame * frameWidth;
        if (++currentFrame >= 5) {
            state = BoltState::Cycling;
            currentFrame = 0;
        }
        break;

    case BoltState::Cycling:
        xOffset = (5 + (currentFrame % 6)) * frameWidth;
        currentFrame++;
        break;

    case BoltState::Ending:
        xOffset = (11 + currentFrame) * frameWidth;
        if (++currentFrame >= 3) {
            state = BoltState::Inactive;
            this->hide();
        }
        break;
    }

    if (!spriteSheet.isNull()) {
        this->setPixmap(spriteSheet.copy(xOffset, 0, frameWidth, frameHeight));
    }
    updateLength();
    checkCollisions();
}

void LightningAttack::startAttack() {
    if (state == BoltState::Inactive || state == BoltState::Ending) {
        state = BoltState::Starting;
        currentFrame = 0;
        this->show();
    }
    AudioManager::instance().playSound("Lightning");
}

void LightningAttack::stopAttack() {
    if (state == BoltState::Starting || state == BoltState::Cycling) {
        state = BoltState::Ending;
        currentFrame = 0;
    }
    AudioManager::instance().stopSound("Lightning");
}

void LightningAttack::checkCollisions() {
    if (state == BoltState::Inactive || state == BoltState::Ending || !scene()) return;

    QPointF worldPos = scenePos();
    qreal thickness = 40;
    qreal length = currentLength;

    QRectF hitBox;
    int angle = static_cast<int>(rotation()) % 360;
    if (angle == 0 || angle == 180 || angle == -180) {
        hitBox = QRectF(worldPos.x() - thickness/2, worldPos.y() - length/2, thickness, length);
    } else {
        hitBox = QRectF(worldPos.x() - length/2, worldPos.y() - thickness/2, length, thickness);
    }

    bool hitSomething = false;

    if (!scene())
        return;

    QList<QGraphicsItem*> hitItems = scene()->items(hitBox);
    for (QGraphicsItem* item : hitItems) {
        if (item == this || dynamic_cast<Characters*>(item)) continue;

        BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(item);
        if (enemy && !enemy->isdead()) {
            enemy->TakeDamage(5);
        }
    }
}

void LightningAttack::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    if (pixmap().isNull()) return;
    painter->save();
    float visualLength = currentLength * 3.0f;

    QRectF clipRect(-frameWidth/2, -frameHeight/2, frameWidth, visualLength);
    painter->setClipRect(clipRect);

    painter->drawPixmap(-frameWidth/2, -frameHeight/2, pixmap());
    painter->restore();
}

QRectF LightningAttack::boundingRect() const {
    return QRectF(-frameWidth/2, -frameHeight/2, frameWidth, frameHeight);
}

void LightningAttack::updateDirection(Direction dir) {
    switch (dir) {
    case Direction::Up:    setRotation(180); break;
    case Direction::Down:  setRotation(0);   break;
    case Direction::Left:  setRotation(90);  break;
    case Direction::Right: setRotation(-90); break;
    }
}

void LightningAttack::updateLength() {
    if (!currentMap) return;

    float maxLength = 130.0f;
    currentLength = maxLength;
    float tileSize = static_cast<float>(currentMap->tileSize());

    QPointF worldPos = scenePos();
    qreal thickness = 40;
    QRectF searchBox;

    int angle = static_cast<int>(rotation()) % 360;
    if (angle == 0)      searchBox = QRectF(worldPos.x() - thickness/2, worldPos.y(), thickness, maxLength);
    else if (angle == 180 || angle == -180) searchBox = QRectF(worldPos.x() - thickness/2, worldPos.y() - maxLength, thickness, maxLength);
    else if (angle == 90)  searchBox = QRectF(worldPos.x() - maxLength, worldPos.y() - thickness/2, maxLength, thickness);
    else                 searchBox = QRectF(worldPos.x(), worldPos.y() - thickness/2, maxLength, thickness);

    int leftCol = static_cast<int>(std::floor(searchBox.left() / tileSize));
    int rightCol = static_cast<int>(std::floor(searchBox.right() / tileSize));
    int topRow = static_cast<int>(std::floor(searchBox.top() / tileSize));
    int bottomRow = static_cast<int>(std::floor(searchBox.bottom() / tileSize));

    float closestDist = maxLength;
    bool hit = false;

    for (int r = topRow; r <= bottomRow; ++r) {
        for (int c = leftCol; c <= rightCol; ++c) {
            if (currentMap->isTileCollidable(r, c)) {
                QPointF tileCenter(c * tileSize + tileSize/2, r * tileSize + tileSize/2);
                float dist = std::sqrt(std::pow(tileCenter.x() - worldPos.x(), 2) +
                                       std::pow(tileCenter.y() - worldPos.y(), 2));

                dist += (tileSize / 2.0f);

                if (dist < closestDist) {
                    closestDist = dist;
                    hit = true;
                }
            }
        }
    }

    const auto &objects = currentMap->getActiveCollidables();
    for (const auto &obj : objects) {
        if (searchBox.intersects(obj.worldHitbox)) {
            float dist = std::sqrt(std::pow(obj.worldHitbox.center().x() - worldPos.x(), 2) +
                                   std::pow(obj.worldHitbox.center().y() - worldPos.y(), 2));
            if (dist < closestDist) {
                closestDist = dist;
                hit = true;
            }
        }
    }

    if (hit) {
        currentLength = closestDist;
    }
}
