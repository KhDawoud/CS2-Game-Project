#include "player.hpp"
#include <QKeyEvent>
#include "AudioManager.hpp"

Player::Player()
{
    health = 100;
    stamina = 100;
    mana = 100;

    staminaRegenRate = 5;
    staminaRegenTimer = new QTimer(this);
    connect(staminaRegenTimer, &QTimer::timeout, [this]()
            { regenStamina(staminaRegenRate); });
    staminaRegenTimer->start(500);

    currentState = PlayerState::Walking;
    currentDirection = Direction::Right;
    currentFrame = 0;

    walkSheet.load(":resources/player/running/swordsman_1_run.png");
    idleSheet.load(":resources/player/idling/swordsman_1_idle.png");
    attackSheet.load(":resources/player/attacking/swordsman_1_attack.png");
    damagedSheet.load(":resources/player/damaged/swordsman_1_damaged.png");

    walkFrameWidth = walkSheet.width() / 8;
    walkFrameHeight = walkSheet.height() / 4;

    idleFrameWidth = idleSheet.width() / 12;
    idleFrameHeight = idleSheet.height() / 4;

    attackFrameWidth = attackSheet.width() / 8;
    attackFrameHeight = attackSheet.height() / 4;

    damagedFrameWidth = damagedSheet.width() / 5;
    damagedFrameHeight = damagedSheet.height() / 4;

    setScale(1.2);

    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &Player::updateAnimation);

    idleTimer = new QTimer(this);
    idleTimer->setSingleShot(true);

    setAnimationState(PlayerState::Idle);

    QTimer *moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Player::movePlayer);
    moveTimer->start(16);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
    setZValue(2);
}

void Player::setAnimationState(PlayerState newState)
{
    if (currentState == newState)
        return;

    currentState = newState;
    currentFrame = 0;

    if (currentState == PlayerState::Idle)
    {
        animTimer->start(200);
    }
    else if (currentState == PlayerState::Attacking || currentState == PlayerState::Damaged)
    {
        animTimer->start(70);
    }
    else
    {
        animTimer->start(100);
    }

    updateAnimation();
}

void Player::updateAnimation()
{
    int currentFrameWidth = 0;
    int currentFrameHeight = 0;
    QPixmap *sheetToDraw = nullptr;

    if (currentState == PlayerState::Idle)
    {
        maxFrames = (currentDirection == Direction::Up) ? 4 : 12;
        currentFrameWidth = idleFrameWidth;
        currentFrameHeight = idleFrameHeight;
        sheetToDraw = &idleSheet;
    }
    else if (currentState == PlayerState::Attacking)
    {
        maxFrames = 8;
        currentFrameWidth = attackFrameWidth;
        currentFrameHeight = attackFrameHeight;
        sheetToDraw = &attackSheet;
        if (currentFrame == 0)
        {
            AudioManager::instance().playSound("SwordSwing");
        }
    }
    else if (currentState == PlayerState::Damaged)
    {
        maxFrames = 5;
        currentFrameWidth = damagedFrameWidth;
        currentFrameHeight = damagedFrameHeight;
        sheetToDraw = &damagedSheet;
        if (currentFrame == 0)
        {
            AudioManager::instance().playSound("DamageTaken");
        }
    }
    else
    {
        maxFrames = 8;
        currentFrameWidth = walkFrameWidth;
        currentFrameHeight = walkFrameHeight;
        sheetToDraw = &walkSheet;

        if (currentFrame % 4 == 0)
        {
            AudioManager::instance().playSound("GrassWalk");
        }
    }

    int row = static_cast<int>(currentDirection);
    setPixmap(sheetToDraw->copy(currentFrame * currentFrameWidth,
                                row * currentFrameHeight,
                                currentFrameWidth,
                                currentFrameHeight));

    if (currentState == PlayerState::Idle && idleTimer->isActive())
    {
        currentFrame = 0;
        return;
    }

    currentFrame++;

    if (currentFrame >= maxFrames)
    {
        currentFrame = 0;

        if (currentState == PlayerState::Attacking || currentState == PlayerState::Damaged)
        {
            idleTimer->start(1000);
            setAnimationState(PlayerState::Idle);
        }
    }
}

void Player::movePlayer()
{
    if (currentState == PlayerState::Attacking || currentState == PlayerState::Damaged)
        return;

    float dx = 0, dy = 0;
    if (activeKeys.contains(Qt::Key_W) || activeKeys.contains(Qt::Key_Up))
        dy -= 1;
    if (activeKeys.contains(Qt::Key_S) || activeKeys.contains(Qt::Key_Down))
        dy += 1;
    if (activeKeys.contains(Qt::Key_A) || activeKeys.contains(Qt::Key_Left))
        dx -= 1;
    if (activeKeys.contains(Qt::Key_D) || activeKeys.contains(Qt::Key_Right))
        dx += 1;

    if (dx == 0 && dy == 0)
    {
        if (currentState == PlayerState::Walking)
        {
            idleTimer->start(1000);
            setAnimationState(PlayerState::Idle);
        }
        return;
    }

    if (idleTimer->isActive())
    {
        idleTimer->stop();
    }
    setAnimationState(PlayerState::Walking);

    if (dx != 0)
    {
        currentDirection = (dx > 0) ? Direction::Right : Direction::Left;
    }
    else if (dy != 0)
    {
        currentDirection = (dy > 0) ? Direction::Down : Direction::Up;
    }

    float speed = (dx != 0 && dy != 0) ? 1.414f : 2.0f;
    setPos(x() + (dx * speed), y() + (dy * speed));
    emit positionChanged(this);
}

void Player::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;
    Qt::Key key = static_cast<Qt::Key>(event->key());

    if (key == Qt::Key_O)
    {
        if (currentState != PlayerState::Damaged)
        {
            idleTimer->stop();
            if (health >= 5)
            {
                setAnimationState(PlayerState::Damaged);
                health -= 5;
                emit statsChanged();
                return;
            }
        }
    }

    if (key == Qt::Key_P)
    {
        if (health + 10 <= 100)
        {
            health += 10;
        }
        emit statsChanged();
    }

    if (key == Qt::Key_Space)
    {
        if (currentState != PlayerState::Attacking)
        {
            idleTimer->stop();
            if (stamina >= 20)
            {
                setAnimationState(PlayerState::Attacking);
                stamina -= 20;
                emit statsChanged();
                return;
            }
        }
        return;
    }

    if (key == Qt::Key_W || key == Qt::Key_A || key == Qt::Key_S || key == Qt::Key_D || key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Left || key == Qt::Key_Right)
    {
        if (!activeKeys.contains(key))
            activeKeys.append(key);
    }
}

void Player::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;
    activeKeys.removeAll(static_cast<Qt::Key>(event->key()));
}
