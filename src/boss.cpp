#include "boss.hpp"
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QTimer>
#include <algorithm>
#include <cmath>

// the boss inherits from a base enemy but overrides update so we can add complex attack patterns
// the whole thing runs on a tick based system where we have a phase timer that counts down and when it hits 0 
// we switch to the next phase and reset the timer based on the new phase.
// the animations update based on both the state and the phase

namespace
{
    constexpr int IdleWaitTicks = 25;
    constexpr int FireballWindupTicks = 15;
    constexpr int FireballCount = 7;
    constexpr qreal FireballSpacing = 32.0;
    constexpr qreal FireballSpawnDistance = 130.0;
    constexpr qreal FireballSpeed = 4.0;
    constexpr qreal FireballHitboxSize = 22.0;
    constexpr int FireballDamage = 20;
    constexpr float BossMeleeRange = 60.0f;

    class BossFireball : public QObject, public QGraphicsPixmapItem
    {
    public:
        BossFireball(const QPointF &startPos, const QPointF &direction, Player *target)
            : player(target), velocity(direction)
        {
            sheet.load(":resources/player/Fireball.png");
            frameWidth = sheet.width() / frameCount;

            setPixmap(sheet.copy(0, 0, frameWidth, sheet.height()));
            setOffset(-frameWidth / 2, -sheet.height() / 2);
            setPos(startPos);
            setScale(1.2);
            setZValue(10000);
            setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

            if (velocity.x() > 0)
                setRotation(0);
            else if (velocity.x() < 0)
                setRotation(180);
            else if (velocity.y() < 0)
                setRotation(270);
            else if (velocity.y() > 0)
                setRotation(90);

            timer = new QTimer(this);
            connect(timer, &QTimer::timeout, this, [this]()
                    { updateFlight(); });

            QTimer::singleShot(3500, this, [this]()
                               {
            if (timer && !timer->isActive())
                deleteLater(); });
        }

        void launch()
        {
            if (!timer->isActive())
                timer->start(16);
        }

    private:
        Player *player = nullptr;
        QPointF velocity;
        QPixmap sheet;
        QTimer *timer = nullptr;
        int frame = 0;
        int frameWidth = 0;
        int frameCount = 5;
        bool hasHitPlayer = false;

        void updateFlight()
        {
            if (!scene())
            {
                deleteLater();
                return;
            }

            setPos(pos() + velocity * FireballSpeed);

            frame = (frame + 1) % frameCount;
            setPixmap(sheet.copy(frame * frameWidth, 0, frameWidth, sheet.height()));

            QRectF hitbox(x() - FireballHitboxSize / 2,
                          y() - FireballHitboxSize / 2,
                          FireballHitboxSize,
                          FireballHitboxSize);

            if (!hasHitPlayer && player && player->scene() == scene() && hitbox.intersects(player->getPlayerHitbox(player->pos())))
            {
                hasHitPlayer = true;
                player->takeDamage(FireballDamage);
                deleteLater();
                return;
            }

            if (!scene()->sceneRect().adjusted(-80, -80, 80, 80).contains(pos()))
            {
                deleteLater();
            }
        }
    };
}

Boss::Boss(int variant)
    : BaseEnemy(
          1000,          // health
          20,            // attack
          10,            // defense
          2.5f,          // speed
          BossMeleeRange, // range
          "BossDamage"), // sound
      currentPhase(BossPhase::IdleWait),
      phaseTimer(0),
      chargeCounter(0),
      nextAttackIsFireball(false),
      fireballsReleased(false),
      chargeSpeedMultiplier(2.5f) // how fast the boss charges
{
    loadAnimations();

    currentState = EnemyState::Idle;
    setScale(2.0);
}

void Boss::loadAnimations()
{
    QString path = ":/resources/boss/";

    idleSheet.load(path + "boss_idle.png");
    walkSheet.load(path + "boss_walk.png");
    attackSheet.load(path + "boss_attack.png");
    deadSheet.load(path + "boss_death.png");
    hurtSheet.load(path + "boss_hurt.png");

    idleData = {idleSheet, 4, 100, 64, 64};
    walkData = {walkSheet, 6, 100, 64, 64};
    attackData = {attackSheet, 8, 100, 64, 64};
    deadData = {deadSheet, 8, 100, 64, 64};
    hurtData = {hurtSheet, 4, 100, 64, 64};
}

QRectF Boss::collisionHitbox() const
{
    QRectF body = sceneBoundingRect();
    return QRectF(body.left() + body.width() * 0.30,
                  body.top() + body.height() * 0.58,
                  body.width() * 0.40,
                  body.height() * 0.34);
}

void Boss::updateDepth()
{
    setZValue(collisionHitbox().bottom());
}

void Boss::facePlayer(float distance)
{
    float diffX = player->x() - x();
    float diffY = player->y() - y();

    if (distance > 0)
    {
        Dir.x = diffX / distance;
        Dir.y = diffY / distance;
    }

    if (std::abs(Dir.x) > std::abs(Dir.y))
        currentRow = (Dir.x > 0) ? 3 : 2;
    else
        currentRow = (Dir.y > 0) ? 0 : 1;
}

void Boss::enterIdleWait()
{
    currentPhase = BossPhase::IdleWait;
    currentState = EnemyState::Idle;
    phaseTimer = 0;
    currentFrame = 0;
    aiTimer->setInterval(100);
}

void Boss::beginNextAttack()
{
    if (nextAttackIsFireball)
        beginFireballAttack();
    else
        beginChargeAttack();

    nextAttackIsFireball = !nextAttackIsFireball;
}

void Boss::beginChargeAttack()
{
    currentPhase = BossPhase::Charging;
    currentState = EnemyState::Walking;
    phaseTimer = 0;
    chargeCounter = 0;
    aiTimer->setInterval(40);
}

void Boss::beginFireballAttack()
{
    currentPhase = BossPhase::FireballWindup;
    currentState = EnemyState::Idle;
    phaseTimer = 0;
    fireballsReleased = false;
    currentFrame = 0;
    aiTimer->setInterval(50);

    int sideRoll = QRandomGenerator::global()->bounded(4);
    spawnFireballWall(static_cast<FireballWallSide>(sideRoll));
}

void Boss::spawnFireballWall(FireballWallSide side)
{
    if (!scene() || !player)
        return;

    QPointF playerCenter = player->sceneBoundingRect().center();
    QPointF direction;
    QPointF base;
    QPointF spacingAxis;

    switch (side)
    {
    case FireballWallSide::Left:
        direction = QPointF(1, 0);
        base = playerCenter + QPointF(-FireballSpawnDistance, 0);
        spacingAxis = QPointF(0, FireballSpacing);
        break;
    case FireballWallSide::Right:
        direction = QPointF(-1, 0);
        base = playerCenter + QPointF(FireballSpawnDistance, 0);
        spacingAxis = QPointF(0, FireballSpacing);
        break;
    case FireballWallSide::Top:
        direction = QPointF(0, 1);
        base = playerCenter + QPointF(0, -FireballSpawnDistance);
        spacingAxis = QPointF(FireballSpacing, 0);
        break;
    case FireballWallSide::Bottom:
        direction = QPointF(0, -1);
        base = playerCenter + QPointF(0, FireballSpawnDistance);
        spacingAxis = QPointF(FireballSpacing, 0);
        break;
    }

    qreal centerOffset = (FireballCount - 1) / 2.0;
    QRectF bounds = scene()->sceneRect().adjusted(16, 16, -16, -16);

    for (int i = 0; i < FireballCount; ++i)
    {
        QPointF pos = base + spacingAxis * (i - centerOffset);
        pos.setX(std::clamp(pos.x(), bounds.left(), bounds.right()));
        pos.setY(std::clamp(pos.y(), bounds.top(), bounds.bottom()));

        auto *fireball = new BossFireball(pos, direction, player);
        scene()->addItem(fireball);
    }
}

void Boss::releaseFireballs()
{
    if (!scene() || fireballsReleased)
        return;

    fireballsReleased = true;

    for (QGraphicsItem *item : scene()->items())
    {
        auto *fireball = dynamic_cast<BossFireball *>(item);
        if (fireball)
            fireball->launch();
    }
}

void Boss::updateIdleWait()
{
    currentState = EnemyState::Idle;
    phaseTimer++;

    if (phaseTimer >= IdleWaitTicks)
        beginNextAttack();
}

void Boss::updateCharging(float distance)
{
    currentState = EnemyState::Walking;
    setPos(x() + Dir.x * (speed * chargeSpeedMultiplier),
           y() + Dir.y * (speed * chargeSpeedMultiplier));

    if (distance <= attackRange)
    {
        currentPhase = BossPhase::Swing1;
        currentState = EnemyState::Attacking;
        phaseTimer = 0;
        currentFrame = 0;
        aiTimer->setInterval(100);
    }
}

void Boss::updateSwing1(float distance)
{
    currentState = EnemyState::Attacking;

    if (currentFrame == 3 && phaseTimer == 0)
    {
        if (distance <= attackRange)
            player->takeDamage(attack);

        phaseTimer = 1;
    }

    if (currentFrame >= attackData.frameCount - 1)
    {
        chargeCounter++;

        if (chargeCounter < 2)
        {
            currentPhase = BossPhase::Charging;
            currentState = EnemyState::Walking;
            aiTimer->setInterval(40);
        }
        else
        {
            enterIdleWait();
        }

        phaseTimer = 0;
        currentFrame = 0;
    }
}

void Boss::updateShortDelay()
{
    currentState = EnemyState::Idle;
    phaseTimer++;

    if (phaseTimer >= 5)
    {
        currentPhase = BossPhase::Swing2;
        currentState = EnemyState::Attacking;
        phaseTimer = 0;
        currentFrame = 0;
    }
}

void Boss::updateSwing2(float distance)
{
    currentState = EnemyState::Attacking;

    if (currentFrame == 3 && phaseTimer == 0)
    {
        if (distance <= attackRange)
            player->takeDamage(attack);

        phaseTimer = 1;
    }

    if (currentFrame >= attackData.frameCount - 1)
    {
        chargeCounter++;

        if (chargeCounter < 2)
        {
            currentPhase = BossPhase::Charging;
            aiTimer->setInterval(40);
        }
        else
        {
            enterIdleWait();
        }

        phaseTimer = 0;
        currentFrame = 0;
    }
}

void Boss::updateFireballWindup()
{
    currentState = EnemyState::Idle;
    phaseTimer++;

    if (phaseTimer >= FireballWindupTicks)
    {
        currentPhase = BossPhase::FireballRelease;
        currentState = EnemyState::Attacking;
        currentFrame = 0;
        phaseTimer = 0;
        aiTimer->setInterval(100);
        releaseFireballs();
    }
}

void Boss::updateFireballRelease()
{
    currentState = EnemyState::Attacking;

    if (currentFrame >= attackData.frameCount - 1)
        enterIdleWait();
}

void Boss::update()
{
    if (!scene() || !player || !player->scene() || scene() != player->scene())
        return;

    if (currentState == EnemyState::Dead)
    {
        updateAnimation();
        updateDepth();
        if (currentFrame >= deadData.frameCount - 1)
        {
            aiTimer->stop();
            hide();
            setEnabled(false);
            emit enemyDied();
        }
        return;
    }

    if (currentState == EnemyState::Hurt)
    {
        updateAnimation();
        updateDepth();
        waitCounter--;
        if (waitCounter <= 0)
        {
            currentState = EnemyState::Idle;
            currentFrame = 0;
            aiTimer->setInterval(100);
        }
        return;
    }

    float diffX = player->x() - x();
    float diffY = player->y() - y();
    float distance = std::sqrt(diffX * diffX + diffY * diffY);
    facePlayer(distance);

    switch (currentPhase)
    {
    case BossPhase::IdleWait:
        updateIdleWait();
        break;
    case BossPhase::Charging:
        updateCharging(distance);
        break;
    case BossPhase::Swing1:
        updateSwing1(distance);
        break;
    case BossPhase::ShortDelay:
        updateShortDelay();
        break;
    case BossPhase::Swing2:
        updateSwing2(distance);
        break;
    case BossPhase::FireballWindup:
        updateFireballWindup();
        break;
    case BossPhase::FireballRelease:
        updateFireballRelease();
        break;
    }

    updateAnimation();
    updateDepth();
}
