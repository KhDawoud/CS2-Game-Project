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

    bool isWaitingToIdle = (currentState == PlayerState::Walking && activeKeys.isEmpty());

    int currentFrameWidth = 0;
    int currentFrameHeight = 0;
    QPixmap *sheetToDraw = nullptr; // A pointer to easily swap which image we use

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
        // so switching directions correctly updates the max frames
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
            idleTimer->start(1000); // we only idle after 0.5 seconds of doing nothing
        }
        return;
    }

    idleTimer->stop();
    setAnimationState(PlayerState::Walking);

    // so keys dont overlap whatever is pressed is added to a list and only
    // most recent input is considered

    Qt::Key newestKey = activeKeys.last();
    int speed = 4;

    switch (newestKey)
    {
    case Qt::Key_Right:
    case Qt::Key_D:
        currentDirection = Direction::Right;
        setPos(x() + speed, y());
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        currentDirection = Direction::Left;
        setPos(x() - speed, y());
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        currentDirection = Direction::Up;
        setPos(x(), y() - speed);
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        currentDirection = Direction::Down;
        setPos(x(), y() + speed);
        break;
    default:
        break;
    }
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