#include "player.hpp"
#include <QKeyEvent>

Player::Player()
{
    currentState = PlayerState::Idle;
    currentDirection = Direction::Right;
    currentFrame = 0;

    walkSheet.load("resources/player/running/swordsman_1_run.png");
    idleSheet.load("resources/player/idling/swordsman_1_idle.png");
    walkFrameWidth = walkSheet.width() / 8;
    walkFrameHeight = walkSheet.height() / 4;

    idleFrameWidth = idleSheet.width() / 12;
    idleFrameHeight = idleSheet.height() / 4;

    setScale(2.0);

    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &Player::updateAnimation);

    idleTimer = new QTimer(this);
    idleTimer->setSingleShot(true);
    connect(idleTimer, &QTimer::timeout, this, [this]()
            { setAnimationState(PlayerState::Idle); });

    setAnimationState(PlayerState::Idle);

    setPixmap(idleSheet.copy(0, static_cast<int>(currentDirection) * idleFrameHeight, idleFrameWidth, idleFrameHeight));

    QTimer *moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Player::movePlayer);
    moveTimer->start(16);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

void Player::setAnimationState(PlayerState newState)
{
    if (currentState == newState)
        return;

    currentState = newState;
    currentFrame = 0;

    if (currentState == PlayerState::Idle)
    {
        animTimer->start(200); // Was cycling too fast so I made it slower
    }
    else
    {
        animTimer->start(100);
    }
}

void Player::updateAnimation()
{
    // since I accept multiple inputs, they can cancel each other out
    bool isMoving = false;
    if (!activeKeys.isEmpty())
    {
        int dx = 0, dy = 0;
        if (activeKeys.contains(Qt::Key_W) || activeKeys.contains(Qt::Key_Up))
            dy -= 1;
        if (activeKeys.contains(Qt::Key_S) || activeKeys.contains(Qt::Key_Down))
            dy += 1;
        if (activeKeys.contains(Qt::Key_A) || activeKeys.contains(Qt::Key_Left))
            dx -= 1;
        if (activeKeys.contains(Qt::Key_D) || activeKeys.contains(Qt::Key_Right))
            dx += 1;

        if (dx != 0 || dy != 0)
        {
            isMoving = true;
        }
    }

    bool isWaitingToIdle = (currentState == PlayerState::Walking && !isMoving);

    int currentFrameWidth = 0;
    int currentFrameHeight = 0;
    QPixmap *sheetToDraw = nullptr;

    if (currentState == PlayerState::Idle || isWaitingToIdle)
    {
        maxFrames = (currentDirection == Direction::Up) ? 4 : 12;
        currentFrameWidth = idleFrameWidth;
        currentFrameHeight = idleFrameHeight;
        sheetToDraw = &idleSheet;
    }
    else
    {
        maxFrames = 8;
        currentFrameWidth = walkFrameWidth;
        currentFrameHeight = walkFrameHeight;
        sheetToDraw = &walkSheet;
    }

    if (isWaitingToIdle)
    {
        currentFrame = 0;
    }
    else
    {
        if (currentFrame >= maxFrames)
        {
            currentFrame = 0;
        }
        currentFrame = (currentFrame + 1) % maxFrames;
    }

    int row = static_cast<int>(currentDirection);
    setPixmap(sheetToDraw->copy(currentFrame * currentFrameWidth, row * currentFrameHeight, currentFrameWidth, currentFrameHeight));
}

void Player::movePlayer()
{
    if (activeKeys.isEmpty())
    {
        if (currentState == PlayerState::Walking && !idleTimer->isActive())
        {
            idleTimer->start(500);
        }
        return;
    }

    // allow multi-directional input
    float dx = 0;
    float dy = 0;

    if (activeKeys.contains(Qt::Key_W) || activeKeys.contains(Qt::Key_Up))
        dy -= 1;
    if (activeKeys.contains(Qt::Key_S) || activeKeys.contains(Qt::Key_Down))
        dy += 1;
    if (activeKeys.contains(Qt::Key_A) || activeKeys.contains(Qt::Key_Left))
        dx -= 1;
    if (activeKeys.contains(Qt::Key_D) || activeKeys.contains(Qt::Key_Right))
        dx += 1;

    // check for cancellation
    if (dx == 0 && dy == 0)
    {
        if (currentState == PlayerState::Walking && !idleTimer->isActive())
        {
            idleTimer->start(500);
        }
        return;
    }

    idleTimer->stop();
    setAnimationState(PlayerState::Walking);

    // since spritesheet only has 4 directions, im js taking x direction for diagonal movement
    if (dx != 0)
    {
        currentDirection = (dx > 0) ? Direction::Right : Direction::Left;
    }
    else if (dy != 0)
    {
        currentDirection = (dy > 0) ? Direction::Down : Direction::Up;
    }

    float baseSpeed = 4.0f;
    float actualSpeed = baseSpeed;

    if (dx != 0 && dy != 0)
    {
        // normalise so diagonal speed same as normal speed
        actualSpeed = baseSpeed * 0.7071f;
    }

    setPos(x() + (dx * actualSpeed), y() + (dy * actualSpeed));
}

void Player::keyPressEvent(QKeyEvent *event)
{
    // so we don't double count holding down the button
    if (event->isAutoRepeat())
        return;
    Qt::Key key = static_cast<Qt::Key>(event->key());

    // for now only adding movement but obv more controls will be added
    if (key == Qt::Key_W || key == Qt::Key_A || key == Qt::Key_S || key == Qt::Key_D ||
        key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Left || key == Qt::Key_Right)
    {
        if (!activeKeys.contains(key))
        {
            activeKeys.append(key);
        }
    }
}

void Player::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;
    Qt::Key key = static_cast<Qt::Key>(event->key());
    activeKeys.removeAll(key);
}