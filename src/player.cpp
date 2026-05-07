#include "player.hpp"
#include <QKeyEvent>
#include "AudioManager.hpp"
#include "map2.hpp"
#include "house_interior-2.hpp"
#include <cmath>
#include "Enemy.hpp"
#include "projectile.hpp"

Player::Player(int charnum): characternum(charnum)
{
    health = 100;
    stamina = 100;
    mana = 100;
    staminaRegenRate = 5;
    manaRegenRate =2;
    staminaRegenTimer = new QTimer(this);
    connect(staminaRegenTimer, &QTimer::timeout, [this]()
            { regenStamina(staminaRegenRate); });
    staminaRegenTimer->start(400);

    manaRegenTimer = new QTimer(this);
    connect(manaRegenTimer, &QTimer::timeout, [this]()
            { regenMana(manaRegenRate); });
    manaRegenTimer->start(400);

    currentState = PlayerState::Walking;
    currentDirection = Direction::Right;
    currentFrame = 0;


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

    rowMap[(int)Direction::Up]    = 3;
    rowMap[(int)Direction::Down]  = 0;
    rowMap[(int)Direction::Left]  = 1;
    rowMap[(int)Direction::Right] = 2;
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
        animTimer->start(70*animationfactor);
    }
    else
    {
        animTimer->start(100*animationfactor);
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
        maxFrames = (currentDirection == Direction::Up) ? idleFrameUpCount : idleFrameWidthCount;
        currentFrameWidth = idleFrameWidth;
        currentFrameHeight = idleFrameHeight;
        sheetToDraw = &idleSheet;
    }
    else if (currentState == PlayerState::Attacking)
    {
        maxFrames = attackFrameWidthCount;
        currentFrameWidth = attackFrameWidth;
        currentFrameHeight = attackFrameHeight;
        sheetToDraw = &attackSheet;
        if (currentFrame == 0 && characternum !=2)
        {
            AudioManager::instance().playSound("SwordSwing");
        }
        if(currentFrame == 3 && characternum ==2 ){
            performAttack();
            AudioManager::instance().playSound("Fireball");
            hasSpawnedFireball = true;
        }
    }
    else if (currentState == PlayerState::Damaged)
    {
        maxFrames = damagedFrameWidthCount;
        currentFrameWidth = damagedFrameWidth;
        currentFrameHeight = damagedFrameHeight;
        sheetToDraw = &damagedSheet;
        if (currentFrame == 0)
        {
            AudioManager::instance().playSound("DamageTaken");
        }
    }
    else if (currentState == PlayerState::Dead)
    {
        maxFrames = deadFrameWidthCount;
        currentFrameWidth = deadFrameWidth;
        currentFrameHeight = deadFrameHeight;
        sheetToDraw = &deadSheet;
        if (currentFrame == 0)
        {
            AudioManager::instance().playSound("DamageTaken");
        }
    }
    else
    {
        maxFrames = walkFrameWidthCount;
        currentFrameWidth = walkFrameWidth;
        currentFrameHeight = walkFrameHeight;
        sheetToDraw = &walkSheet;

        if (currentFrame % 4 == 0)
        {
            AudioManager::instance().playSound("GrassWalk");
        }
    }

    int row = static_cast<int>(rowMap[(int)currentDirection]);
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

        if (currentState == PlayerState::Dead)
        {
            AudioManager::instance().playSound("GameOver");
            emit playerDied();
        }

        if (currentState == PlayerState::Attacking || currentState == PlayerState::Damaged)
        {
            idleTimer->start(1000);
            setAnimationState(PlayerState::Idle);
            hasSpawnedFireball = false;
        }
    }
}

void Player::movePlayer()
{
    if (currentState == PlayerState::Attacking || currentState == PlayerState::Damaged || currentState == PlayerState::Dead)
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
        currentDirection = (dx > 0) ? Direction::Right : Direction::Left;
    else if (dy != 0)
        currentDirection = (dy > 0) ? Direction::Down : Direction::Up;

    float speed = (dx != 0 && dy != 0) ? 1.414f : 2.0f;

    Map *map = dynamic_cast<Map *>(scene());
    House_Interior *interior = dynamic_cast<House_Interior *>(scene());

    float newX = x() + (dx * speed);
    float newY = y() + (dy * speed);

    if (dx != 0)
    {
        QRectF predictedHitboxX = getPlayerHitbox(QPointF(newX, y()));
        if (!checkCollision(predictedHitboxX, map, interior))
            setX(newX);
    }
    if (dy != 0)
    {
        QRectF predictedHitboxY = getPlayerHitbox(QPointF(x(), newY));
        if (!checkCollision(predictedHitboxY, map, interior))
            setY(newY);
    }

    QRectF actualHitbox = getPlayerHitbox(pos());
    setZValue(actualHitbox.bottom());

    if (!debugHitboxItem && scene())
    {
        debugHitboxItem = new QGraphicsRectItem();
        debugHitboxItem->setBrush(QBrush(QColor(0, 0, 255, 100)));
        debugHitboxItem->setPen(QPen(Qt::blue));
        debugHitboxItem->setZValue(10000);
        scene()->addItem(debugHitboxItem);
    }
    if (debugHitboxItem)
    {
        debugHitboxItem->setRect(actualHitbox);
    }

    emit positionChanged(this);
}

void Player::takeDamage(float damage)
{
    if (currentState != PlayerState::Damaged)
    {
        idleTimer->stop();
        if (health >= damage)
        {
            setAnimationState(PlayerState::Damaged);
            health -= damage;
            emit statsChanged();
            return;
        }
        else
        {
            setAnimationState(PlayerState::Dead);
        }
    }
}

void Player::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat() || currentState == PlayerState::Dead)
        return;
    Qt::Key key = static_cast<Qt::Key>(event->key());

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
            if(stamina >=20){
                if(mana >= 20 && characternum==2){
                    setAnimationState(PlayerState::Attacking);
                    stamina -= 20;
                    mana -=20;
                    emit statsChanged();
                    return;
                }
                else if(characternum!=2)
                {
                    setAnimationState(PlayerState::Attacking);
                    stamina -= 20;
                    performAttack();
                    emit statsChanged();
                    return;
                }
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

QRectF Player::getPlayerHitbox(QPointF pos) const
{
    float hitboxWidth, hitboxHeight, offsetX, offsetY;
        hitboxWidth = 15.0f;
        hitboxHeight = 15.0f;
        offsetX = 30.0f;
        offsetY = 40.0f;


    return QRectF(pos.x() + offsetX, pos.y() + offsetY, hitboxWidth, hitboxHeight);
}

bool Player::checkCollision(const QRectF &hitbox, Map *map, House_Interior *interior) const
{
    if (hitbox.left() < 0 || hitbox.top() < 0)
        return true;

    float tileSize = 32.0f;

    int leftCol = static_cast<int>(std::floor(hitbox.left() / tileSize));
    int rightCol = static_cast<int>(std::floor(hitbox.right() / tileSize));
    int topRow = static_cast<int>(std::floor(hitbox.top() / tileSize));
    int bottomRow = static_cast<int>(std::floor(hitbox.bottom() / tileSize));

    if (map)
    {
        for (int r = topRow; r <= bottomRow; ++r)
            for (int c = leftCol; c <= rightCol; ++c)
                if (map->isTileCollidable(r, c))
                    return true;

        for (const auto &obj : map->getCollidableObjects())
            if (hitbox.intersects(obj.worldHitbox))
                return true;
    }
    else if (interior)
    {
        for (int r = topRow; r <= bottomRow; ++r)
            for (int c = leftCol; c <= rightCol; ++c)
                if (interior->isTileCollidable(r, c))
                    return true;

        for (const auto &obj : interior->getCollidableObjects())
            if (hitbox.intersects(obj.worldHitbox))
                return true;
    }

    return false;
}

void Player::performAttack()
{
    if (!scene())
        return;
    if(characternum !=2){

        QRectF pBox = getPlayerHitbox(pos());
        QRectF attackRect;

        if (currentDirection == Direction::Right)
            attackRect = QRectF(pBox.right(), pBox.center().y() - 20, 20, 40);
        else if (currentDirection == Direction::Left)
            attackRect = QRectF(pBox.left() - 20, pBox.center().y() - 20, 20, 40);
        else if (currentDirection == Direction::Up)
            attackRect = QRectF(pBox.center().x() - 20, pBox.top() - 20, 40, 20);
        else if (currentDirection == Direction::Down)
            attackRect = QRectF(pBox.center().x() - 20, pBox.bottom(), 40, 20);

    QList<QGraphicsItem *> hitItems = scene()->items(attackRect);
    for (QGraphicsItem *item : hitItems)
    {
        BaseEnemy *enemy = dynamic_cast<BaseEnemy *>(item);
        if (enemy && !enemy->isdead())
            enemy->TakeDamage(damage);
    }
    }else{
        shootFireball();
    }
}

void Player::Heal(float amount)
{
    health += amount;
    health = (health > 100) ? 100 : health;
    AudioManager::instance().playSound("Heal");
    emit statsChanged();
}

int Player::getcharacternum(){
    return characternum;
}

void Player::shootFireball() {
    QPointF playerCenter = this->sceneBoundingRect().center();

    QPointF shootDir(0, 0);
    qreal forwardOffset = 20.0; // How far in front of the player to spawn

    if (currentDirection == Direction::Right) {
        shootDir = QPointF(1, 0);
    }
    else if (currentDirection == Direction::Left) {
        shootDir = QPointF(-1, 0);
    }
    else if (currentDirection == Direction::Up) {
        shootDir = QPointF(0, -1);
    }
    else if (currentDirection == Direction::Down) {
        shootDir = QPointF(0, 1);
    }

    QPointF spawnPos = playerCenter + (shootDir * forwardOffset);

    Map *map = dynamic_cast<Map *>(scene());
    House_Interior *interior = dynamic_cast<House_Interior *>(scene());
    Projectile *fireball = new Projectile(spawnPos, shootDir, map, interior);
    this->scene()->addItem(fireball);
}




