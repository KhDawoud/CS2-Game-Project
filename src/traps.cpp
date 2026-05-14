#include "traps.hpp"
#include "player.hpp"

#include <QGraphicsScene>
#include <QTimer>
#include <QTransform>
#include <QtGlobal>

//Contains the implementation of all the traps that are utilized in level 2. This includes
//animation, creating the damage area and checking for overlap between traps and the player.

static QRectF playerDamageBox(Player *player)
{
    QRectF box = player->sceneBoundingRect();

    box.adjust(box.width() * 0.25, box.height() * 0.25, -box.width() * 0.25, -box.height() * 0.10);

    return box;
}

// ===========================================================================
// SpikeTrap
// ===========================================================================

SpikeTrap::SpikeTrap(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , frameWidth(0)
    , frameHeight(0)
    , currentFrame(0)
    , displayedFrame(0)
    , damage(15.0f)
    , canDamage(true)
    , animationTimer(new QTimer(this))
    , damageTimer(new QTimer(this))
    , cooldownTimer(new QTimer(this))
    , pauseTimer(new QTimer(this))
    , isPaused(false)
{
    framesheet.load(":resources/traps/trap1.png");
    qDebug() << "SpikeTrap framesheet loaded:" << !framesheet.isNull();

    frameWidth = framesheet.width();
    frameHeight = framesheet.height() / frameCount;

    setScale(2.0);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    UpdateAnimation();

    connect(animationTimer, &QTimer::timeout, this, &SpikeTrap::UpdateAnimation);
    animationTimer->start(150);

    connect(damageTimer, &QTimer::timeout, this, &SpikeTrap::CheckDamage);
    damageTimer->start(60);

    cooldownTimer->setSingleShot(true);
    cooldownTimer->setInterval(800);

    connect(cooldownTimer, &QTimer::timeout, this, [this]() {
        if (!isPaused)
            canDamage = true;
    });

    pauseTimer->setSingleShot(true);
    connect(pauseTimer, &QTimer::timeout, this, [this]() {
        isPaused = false;
        canDamage = true;
        currentFrame = 0;
        animationTimer->start(150);
    });
}

void SpikeTrap::UpdateAnimation()
{
    if (isPaused)
        return;

    displayedFrame = currentFrame;

    int y = displayedFrame * frameHeight;
    setPixmap(framesheet.copy(0, y, frameWidth, frameHeight));

    if (currentFrame >= frameCount - 1) {
        isPaused = true;
        canDamage = false;
        currentFrame = 0;
        displayedFrame = 0;
        setPixmap(framesheet.copy(0, 0, frameWidth, frameHeight));
        animationTimer->stop();
        pauseTimer->start(2000);
        return;
    }

    currentFrame++;
}

void SpikeTrap::CheckDamage()
{
    if (!scene() || !canDamage || isPaused || !isExtended())
        return;

    QRectF area = damageArea();

    for (QGraphicsItem *item : scene()->items(area, Qt::IntersectsItemShape)) {
        Player *player = dynamic_cast<Player *>(item);

        if (player && area.intersects(playerDamageBox(player))) {
            applyDamageTo(player);
            return;
        }
    }
}
/*
QRectF SpikeTrap::damageArea() const
{
    QRectF area = sceneBoundingRect();

    area.adjust(area.width() * 0.50,
                area.height() * 0.35,
                -area.width() * 0.50,
                -area.height() * 0.05);

    return area;
}*/

QRectF SpikeTrap::damageArea() const
{
    QPointF origin = scenePos();
    return QRectF(
        origin.x() + 48,   // offset from left
        origin.y() + 48,   // offset from top
        32,                 // width
        16                  // height
        );
}

bool SpikeTrap::isExtended() const
{
    return displayedFrame >= frameCount - 2;
}

void SpikeTrap::applyDamageTo(Player *p)
{
    p->takeDamage(damage);
    canDamage = false;
    cooldownTimer->start();
}

int SpikeTrap::getwidth() const
{
    return int(frameWidth * scale());
}

int SpikeTrap::getheight() const
{
    return int(frameHeight * scale());
}

void SpikeTrap::setDamage(float amount)
{
    damage = amount;
}

void SpikeTrap::setDamageCooldown(int milliseconds)
{
    cooldownTimer->setInterval(milliseconds);
}

// ===========================================================================
// SlidingSpikeTrap
// ===========================================================================

SlidingSpikeTrap::SlidingSpikeTrap(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , frameWidth(0)
    , frameHeight(0)
    , currentFrame(0)
    , displayedFrame(0)
    , direction(1)
    , damage(12.0f)
    , canDamage(true)
    , animationTimer(new QTimer(this))
    , damageTimer(new QTimer(this))
    , cooldownTimer(new QTimer(this))
    , pauseTimer(new QTimer(this))
    , isPaused(false)
{
    framesheet.load(":resources/traps/trap2.png");

    frameWidth = framesheet.width();
    frameHeight = framesheet.height() / frameCount;

    setScale(2.0);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    UpdateAnimation();

    connect(animationTimer, &QTimer::timeout, this, &SlidingSpikeTrap::UpdateAnimation);
    animationTimer->start(220);

    connect(damageTimer, &QTimer::timeout, this, &SlidingSpikeTrap::CheckDamage);
    damageTimer->start(60);

    cooldownTimer->setSingleShot(true);
    cooldownTimer->setInterval(700);

    connect(cooldownTimer, &QTimer::timeout, this, [this]() {
        if (!isPaused)
            canDamage = true;
    });

    pauseTimer->setSingleShot(true);
    connect(pauseTimer, &QTimer::timeout, this, [this]() {
        isPaused = false;
        canDamage = true;
        currentFrame = 0;
        direction = 1;
        animationTimer->start(220);
    });
}

void SlidingSpikeTrap::UpdateAnimation()
{
    if (isPaused)
        return;

    displayedFrame = currentFrame;

    int y = displayedFrame * frameHeight;
    setPixmap(framesheet.copy(0, y, frameWidth, frameHeight));

    currentFrame += direction;

    if (currentFrame >= frameCount - 1) {
        currentFrame = frameCount - 1;
        direction = -1;
    } else if (currentFrame <= 0 && direction == -1) {
        currentFrame = 0;
        direction = 1;
        displayedFrame = 0;
        setPixmap(framesheet.copy(0, 0, frameWidth, frameHeight));

        isPaused = true;
        canDamage = false;
        animationTimer->stop();
        pauseTimer->start(2000);
    }
}

void SlidingSpikeTrap::CheckDamage()
{
    if (!scene() || !canDamage || isPaused)
        return;

    QRectF area = damageArea();

    for (QGraphicsItem *item : scene()->items(area, Qt::IntersectsItemShape)) {
        Player *player = dynamic_cast<Player *>(item);

        if (player && area.intersects(playerDamageBox(player))) {
            applyDamageTo(player);
            return;
        }
    }
}

QRectF SlidingSpikeTrap::spikeLocalRect() const
{
    qreal headW = frameWidth / qreal(frameCount);
    qreal x = (frameWidth - headW) * (displayedFrame / qreal(frameCount - 1));

    return QRectF(x + headW * 0.20, frameHeight * 0.30, headW * 0.60, frameHeight * 0.45);
}

QRectF SlidingSpikeTrap::damageArea() const
{
    return mapRectToScene(spikeLocalRect());
}

void SlidingSpikeTrap::applyDamageTo(Player *p)
{
    p->takeDamage(damage);
    canDamage = false;
    cooldownTimer->start();
}

int SlidingSpikeTrap::getwidth() const
{
    return int(frameWidth * scale());
}

int SlidingSpikeTrap::getheight() const
{
    return int(frameHeight * scale());
}

void SlidingSpikeTrap::setDamage(float amount)
{
    damage = amount;
}

void SlidingSpikeTrap::setDamageCooldown(int milliseconds)
{
    cooldownTimer->setInterval(milliseconds);
}

// ===========================================================================
// SawBladeTrap
// ===========================================================================

SawBladeTrap::SawBladeTrap(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , frameWidth(0)
    , frameHeight(0)
    , currentFrame(0)
    , displayedFrame(0)
    , damage(20.0f)
    , canDamage(true)
    , animationTimer(new QTimer(this))
    , damageTimer(new QTimer(this))
    , cooldownTimer(new QTimer(this))
    , pauseTimer(new QTimer(this))
    , isPaused(false)
{
    framesheet.load(":resources/traps/trap3.png");

    frameHeight = framesheet.height();
    frameWidth = framesheet.width() / frameCount;

    setScale(2.0);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    UpdateAnimation();

    connect(animationTimer, &QTimer::timeout, this, &SawBladeTrap::UpdateAnimation);
    animationTimer->start(70);

    connect(damageTimer, &QTimer::timeout, this, &SawBladeTrap::CheckDamage);
    damageTimer->start(50);

    cooldownTimer->setSingleShot(true);
    cooldownTimer->setInterval(550);

    connect(cooldownTimer, &QTimer::timeout, this, [this]() {
        if (!isPaused)
            canDamage = true;
    });

    pauseTimer->setSingleShot(true);
    connect(pauseTimer, &QTimer::timeout, this, [this]() {
        isPaused = false;
        canDamage = true;
        currentFrame = 0;
        animationTimer->start(70);
    });
}

void SawBladeTrap::UpdateAnimation()
{
    if (isPaused)
        return;

    displayedFrame = currentFrame;

    int x = qRound(displayedFrame * (framesheet.width() / qreal(frameCount)));
    setPixmap(framesheet.copy(x, 0, frameWidth, frameHeight));

    if (currentFrame >= frameCount - 1) {
        isPaused = true;
        canDamage = false;
        currentFrame = 0;
        displayedFrame = 0;
        setPixmap(framesheet.copy(0, 0, frameWidth, frameHeight));
        animationTimer->stop();
        pauseTimer->start(2000);
        return;
    }

    currentFrame++;
}

void SawBladeTrap::CheckDamage()
{
    if (!scene() || !canDamage || isPaused)
        return;

    QRectF area = damageArea();
    for (QGraphicsItem *item : scene()->items(area, Qt::IntersectsItemShape)) {
        Player *player = dynamic_cast<Player *>(item);

        if (player && area.intersects(playerDamageBox(player))) {
            applyDamageTo(player);
            return;
        }
    }
}
/*
QRectF SawBladeTrap::damageArea() const
{
    QRectF area = sceneBoundingRect();

    area.adjust(area.width() * 0.20,
                area.height() * 0.15,
                -area.width() * 0.20,
                -area.height() * 0.25);

    return area;
}*/

QRectF SawBladeTrap::damageArea() const
{
    QPointF origin = scenePos();
    return QRectF(
        origin.x() + 20,   // offset from left
        origin.y() + 24,   // offset from top
        36,                 // width
        30                  // height
        );
}

void SawBladeTrap::applyDamageTo(Player *p)
{
    p->takeDamage(damage);
    canDamage = false;
    cooldownTimer->start();
}

int SawBladeTrap::getwidth() const
{
    return int(frameWidth * scale());
}

int SawBladeTrap::getheight() const
{
    return int(frameHeight * scale());
}

void SawBladeTrap::setDamage(float amount)
{
    damage = amount;
}

void SawBladeTrap::setDamageCooldown(int milliseconds)
{
    cooldownTimer->setInterval(milliseconds);
}

// ===========================================================================
// FireTrap - shoots LEFT
// ===========================================================================

FireTrap::FireTrap(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , frameWidth(0)
    , frameHeight(0)
    , currentFrame(0)
    , displayedFrame(0)
    , sparkDamage(3.0f)
    , canDamage(true)
    , animationTimer(new QTimer(this))
    , damageTimer(new QTimer(this))
    , cooldownTimer(new QTimer(this))
    , pauseTimer(new QTimer(this))
    , isPaused(false)
{
    framesheet.load(":resources/traps/fire_trap1.png");

    frameWidth = framesheet.width() / frameCount;
    frameHeight = framesheet.height();

    setScale(2.0);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    UpdateAnimation();

    connect(animationTimer, &QTimer::timeout, this, &FireTrap::UpdateAnimation);
    animationTimer->start(100);

    connect(damageTimer, &QTimer::timeout, this, &FireTrap::CheckDamage);
    damageTimer->start(60);

    cooldownTimer->setSingleShot(true);
    cooldownTimer->setInterval(700);

    connect(cooldownTimer, &QTimer::timeout, this, [this]() {
        if (!isPaused)
            canDamage = true;
    });

    pauseTimer->setSingleShot(true);
    connect(pauseTimer, &QTimer::timeout, this, [this]() {
        isPaused = false;
        canDamage = true;
        currentFrame = 0;
        animationTimer->start(100);
    });
}

void FireTrap::UpdateAnimation()
{
    if (isPaused)
        return;

    displayedFrame = currentFrame;

    setPixmap(framesheet.copy(currentFrame * frameWidth, 0, frameWidth, frameHeight));

    if (currentFrame >= frameCount - 1) {
        isPaused = true;
        canDamage = false;
        currentFrame = 0;
        displayedFrame = 0;
        setPixmap(framesheet.copy(0, 0, frameWidth, frameHeight));
        animationTimer->stop();
        pauseTimer->start(2000);
        return;
    }

    currentFrame++;
}

bool FireTrap::isSparkFrame() const
{
    return displayedFrame >= 1 && displayedFrame <= 3;
}

bool FireTrap::isFireFrame() const
{
    return displayedFrame >= 4;
}

void FireTrap::CheckDamage()
{
    if (!scene() || !canDamage || isPaused)
        return;

    if (!isSparkFrame() && !isFireFrame())
        return;

    QRectF area = damageArea();

    for (QGraphicsItem *item : scene()->items(area, Qt::IntersectsItemShape)) {
        Player *player = dynamic_cast<Player *>(item);

        if (player && area.intersects(playerDamageBox(player))) {
            applyDamageTo(player);
            return;
        }
    }
}

void FireTrap::applyDamageTo(Player *player)
{
    if (isFireFrame()) {
        player->takeDamage(player->getHealth() * 0.25f);
        cooldownTimer->setInterval(600);
    } else {
        player->takeDamage(sparkDamage);
        cooldownTimer->setInterval(200);
    }

    canDamage = false;
    cooldownTimer->start();
}

QRectF FireTrap::damageArea() const
{
    if (!isSparkFrame() && !isFireFrame())
        return QRectF();

    QRectF area = sceneBoundingRect();

    qreal h = area.height() * 0.25;
    qreal y = area.center().y() - h / 2;

    qreal w = isSparkFrame() ? area.width() * 0.22 : area.width() * 0.40;

    return QRectF(area.right() - w - area.width() * 0.12, y, w, h);
}

int FireTrap::getwidth() const
{
    return int(frameWidth * scale());
}

int FireTrap::getheight() const
{
    return int(frameHeight * scale());
}

// ===========================================================================
// FireTrap2 - shoots RIGHT
// ===========================================================================

FireTrap2::FireTrap2(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , frameWidth(0)
    , frameHeight(0)
    , currentFrame(0)
    , displayedFrame(0)
    , sparkDamage(3.0f)
    , canDamage(true)
    , animationTimer(new QTimer(this))
    , damageTimer(new QTimer(this))
    , cooldownTimer(new QTimer(this))
    , pauseTimer(new QTimer(this))
    , isPaused(false)
{
    framesheet.load(":resources/traps/fire_trap1.png");

    frameWidth = framesheet.width() / frameCount;
    frameHeight = framesheet.height();

    setScale(2.0);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    UpdateAnimation();

    connect(animationTimer, &QTimer::timeout, this, &FireTrap2::UpdateAnimation);
    animationTimer->start(100);

    connect(damageTimer, &QTimer::timeout, this, &FireTrap2::CheckDamage);
    damageTimer->start(60);

    cooldownTimer->setSingleShot(true);
    cooldownTimer->setInterval(700);

    connect(cooldownTimer, &QTimer::timeout, this, [this]() {
        if (!isPaused)
            canDamage = true;
    });

    pauseTimer->setSingleShot(true);
    connect(pauseTimer, &QTimer::timeout, this, [this]() {
        isPaused = false;
        canDamage = true;
        currentFrame = 0;
        animationTimer->start(100);
    });
}

void FireTrap2::UpdateAnimation()
{
    if (isPaused)
        return;

    displayedFrame = currentFrame;

    QPixmap frame = framesheet.copy(currentFrame * frameWidth, 0, frameWidth, frameHeight);

    setPixmap(frame.transformed(QTransform().scale(-1, 1)));

    if (currentFrame >= frameCount - 1) {
        isPaused = true;
        canDamage = false;
        currentFrame = 0;
        displayedFrame = 0;
        QPixmap firstFrame = framesheet.copy(0, 0, frameWidth, frameHeight);
        setPixmap(firstFrame.transformed(QTransform().scale(-1, 1)));
        animationTimer->stop();
        pauseTimer->start(2000);
        return;
    }

    currentFrame++;
}

bool FireTrap2::isSparkFrame() const
{
    return displayedFrame >= 1 && displayedFrame <= 3;
}

bool FireTrap2::isFireFrame() const
{
    return displayedFrame >= 4;
}

void FireTrap2::CheckDamage()
{
    if (!scene() || !canDamage || isPaused)
        return;

    if (!isSparkFrame() && !isFireFrame())
        return;

    QRectF area = damageArea();


    for (QGraphicsItem *item : scene()->items(area, Qt::IntersectsItemShape)) {
        Player *player = dynamic_cast<Player *>(item);

        if (player && area.intersects(playerDamageBox(player))) {
            applyDamageTo(player);
            return;
        }
    }
}

void FireTrap2::applyDamageTo(Player *player)
{
    if (isFireFrame()) {
        player->takeDamage(player->getHealth() * 0.25f);
        cooldownTimer->setInterval(600);
    } else {
        player->takeDamage(sparkDamage);
        cooldownTimer->setInterval(200);
    }

    canDamage = false;
    cooldownTimer->start();
}

QRectF FireTrap2::damageArea() const
{
    if (!isSparkFrame() && !isFireFrame())
        return QRectF();

    QRectF area = sceneBoundingRect();

    qreal h = area.height() * 0.25;
    qreal y = area.center().y() - h / 2;
    qreal w = isSparkFrame() ? area.width() * 0.22 : area.width() * 0.40;

    return QRectF(area.left() + area.width() * 0.12, y, w, h);
}

int FireTrap2::getwidth() const
{
    return int(frameWidth * scale());
}

int FireTrap2::getheight() const
{
    return int(frameHeight * scale());
}

// ===========================================================================
// FireTrap3 - shoots UP
// ===========================================================================

FireTrap3::FireTrap3(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , frameWidth(0)
    , frameHeight(0)
    , currentFrame(0)
    , displayedFrame(0)
    , sparkDamage(3.0f)
    , canDamage(true)
    , animationTimer(new QTimer(this))
    , damageTimer(new QTimer(this))
    , cooldownTimer(new QTimer(this))
    , pauseTimer(new QTimer(this))
    , isPaused(false)
{
    framesheet.load(":resources/traps/fire_trap3.png");

    frameWidth = framesheet.width() / frameCount;
    frameHeight = framesheet.height();

    setScale(2.0);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    UpdateAnimation();

    connect(animationTimer, &QTimer::timeout, this, &FireTrap3::UpdateAnimation);
    animationTimer->start(100);

    connect(damageTimer, &QTimer::timeout, this, &FireTrap3::CheckDamage);
    damageTimer->start(60);

    cooldownTimer->setSingleShot(true);
    cooldownTimer->setInterval(700);

    connect(cooldownTimer, &QTimer::timeout, this, [this]() {
        if (!isPaused)
            canDamage = true;
    });

    pauseTimer->setSingleShot(true);
    connect(pauseTimer, &QTimer::timeout, this, [this]() {
        isPaused = false;
        canDamage = true;
        currentFrame = 0;
        animationTimer->start(100);
    });
}

void FireTrap3::UpdateAnimation()
{
    if (isPaused)
        return;

    displayedFrame = currentFrame;

    setPixmap(framesheet.copy(currentFrame * frameWidth, 0, frameWidth, frameHeight));

    if (currentFrame >= frameCount - 1) {
        isPaused = true;
        canDamage = false;
        currentFrame = 0;
        displayedFrame = 0;
        setPixmap(framesheet.copy(0, 0, frameWidth, frameHeight));
        animationTimer->stop();
        pauseTimer->start(2000);
        return;
    }

    currentFrame++;
}

bool FireTrap3::isSparkFrame() const
{
    return displayedFrame >= 1 && displayedFrame <= 3;
}

bool FireTrap3::isFireFrame() const
{
    return displayedFrame >= 4;
}

void FireTrap3::CheckDamage()
{
    if (!scene() || !canDamage || isPaused)
        return;

    if (!isSparkFrame() && !isFireFrame())
        return;

    QRectF area = damageArea();

    for (QGraphicsItem *item : scene()->items(area, Qt::IntersectsItemShape)) {
        Player *player = dynamic_cast<Player *>(item);

        if (player && area.intersects(playerDamageBox(player))) {
            applyDamageTo(player);
            return;
        }
    }
}

void FireTrap3::applyDamageTo(Player *player)
{
    if (isFireFrame()) {
        player->takeDamage(player->getHealth() * 0.25f);
        cooldownTimer->setInterval(600);
    } else {
        player->takeDamage(sparkDamage);
        cooldownTimer->setInterval(200);
    }

    canDamage = false;
    cooldownTimer->start();
}

QRectF FireTrap3::damageArea() const
{
    QPointF origin = mapToScene(QPointF(0, 0));
    qreal w = 24;
    qreal h = isSparkFrame() ? 32 : 80;
    return QRectF(
        origin.x() + 6*32 + 6,
        origin.y() - h + 32*3,
        w,
        h
        );
}

int FireTrap3::getwidth() const
{
    return int(frameWidth * scale());
}

int FireTrap3::getheight() const
{
    return int(frameHeight * scale());
}

// ===========================================================================
// FireTrap4 - shoots DOWN
// ===========================================================================

FireTrap4::FireTrap4(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , frameWidth(0)
    , frameHeight(0)
    , currentFrame(0)
    , displayedFrame(0)
    , sparkDamage(3.0f)
    , canDamage(true)
    , animationTimer(new QTimer(this))
    , damageTimer(new QTimer(this))
    , cooldownTimer(new QTimer(this))
    , pauseTimer(new QTimer(this))
    , isPaused(false)
{
    framesheet.load(":resources/traps/fire_trap3.png");

    frameWidth = framesheet.width() / frameCount;
    frameHeight = framesheet.height();

    setScale(2.0);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    UpdateAnimation();

    connect(animationTimer, &QTimer::timeout, this, &FireTrap4::UpdateAnimation);
    animationTimer->start(100);

    connect(damageTimer, &QTimer::timeout, this, &FireTrap4::CheckDamage);
    damageTimer->start(60);

    cooldownTimer->setSingleShot(true);
    cooldownTimer->setInterval(700);

    connect(cooldownTimer, &QTimer::timeout, this, [this]() {
        if (!isPaused)
            canDamage = true;
    });

    pauseTimer->setSingleShot(true);
    connect(pauseTimer, &QTimer::timeout, this, [this]() {
        isPaused = false;
        canDamage = true;
        currentFrame = 0;
        animationTimer->start(100);
    });
}

void FireTrap4::UpdateAnimation()
{
    if (isPaused)
        return;

    displayedFrame = currentFrame;

    QPixmap frame = framesheet.copy(currentFrame * frameWidth, 0, frameWidth, frameHeight);

    setPixmap(frame.transformed(QTransform().scale(1, -1)));

    if (currentFrame >= frameCount - 1) {
        isPaused = true;
        canDamage = false;
        currentFrame = 0;
        displayedFrame = 0;
        QPixmap firstFrame = framesheet.copy(0, 0, frameWidth, frameHeight);
        setPixmap(firstFrame.transformed(QTransform().scale(1, -1)));
        animationTimer->stop();
        pauseTimer->start(2000);
        return;
    }

    currentFrame++;
}

bool FireTrap4::isSparkFrame() const
{
    return displayedFrame >= 1 && displayedFrame <= 3;
}

bool FireTrap4::isFireFrame() const
{
    return displayedFrame >= 4;
}

void FireTrap4::CheckDamage()
{
    if (!scene() || !canDamage || isPaused)
        return;

    if (!isSparkFrame() && !isFireFrame())
        return;

    QRectF area = damageArea();


    for (QGraphicsItem *item : scene()->items(area, Qt::IntersectsItemShape)) {
        Player *player = dynamic_cast<Player *>(item);

        if (player && area.intersects(playerDamageBox(player))) {
            applyDamageTo(player);
            return;
        }
    }
}

void FireTrap4::applyDamageTo(Player *player)
{
    if (isFireFrame()) {
        player->takeDamage(player->getHealth() * 0.25f);
        cooldownTimer->setInterval(600);
    } else {
        player->takeDamage(sparkDamage);
        cooldownTimer->setInterval(200);
    }

    canDamage = false;
    cooldownTimer->start();
}

QRectF FireTrap4::damageArea() const
{
    QPointF origin = scenePos();
    qreal w = 24;
    qreal h = isSparkFrame() ? 32 : 80;
    return QRectF(
        origin.x() + 6*32 + 6,
        origin.y() + 32,
        w,
        h
        );
}

int FireTrap4::getwidth() const
{
    return int(frameWidth * scale());
}

int FireTrap4::getheight() const
{
    return int(frameHeight * scale());
}
