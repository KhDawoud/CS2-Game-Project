#include "Enemy.hpp"
#include <QDebug>
#include <QTimer>
#include <QTransform>
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <QPoint>
#include <QPointF>
#include <string>
#include <cmath>
#include <queue>
#include <vector>
#include "player.hpp"
#include "AudioManager.hpp"
#include "Heart.hpp"
#include "maploader.hpp"

using namespace std;

namespace
{
QPointF tileCenter(int row, int col, int tileSize)
{
    // Aim enemies at the middle of a tile so path movement does not wobble around tile edges.
    return QPointF((col + 0.5f) * tileSize, (row + 0.5f) * tileSize);
}

bool pathTileBlocked(MapLoader *map, int row, int col)
{
    // Pathfinding treats both solid map tiles and placed objects as things to route around.
    if (map->isTileCollidable(row, col))
        return true;

    QRectF tileRect(col * map->tileSize(), row * map->tileSize(), map->tileSize(), map->tileSize());
    for (const auto &obj : map->getActiveCollidables())
    {
        if (tileRect.intersects(obj.worldHitbox))
            return true;
    }

    return false;
}

bool nextPathPoint(MapLoader *map, const QPointF &from, const QPointF &to, QPointF &nextPoint)
{
    // Breadth-first search is enough for these small tile maps and gives the nearest route.
    const int rows = map->mapRows();
    const int cols = map->mapCols();
    const int tileSize = map->tileSize();

    int startRow = static_cast<int>(std::floor(from.y() / tileSize));
    int startCol = static_cast<int>(std::floor(from.x() / tileSize));
    int goalRow = static_cast<int>(std::floor(to.y() / tileSize));
    int goalCol = static_cast<int>(std::floor(to.x() / tileSize));

    if (startRow < 0 || startRow >= rows || startCol < 0 || startCol >= cols ||
        goalRow < 0 || goalRow >= rows || goalCol < 0 || goalCol >= cols)
    {
        return false;
    }

    if (startRow == goalRow && startCol == goalCol)
        return false;

    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::vector<std::vector<QPoint>> parent(rows, std::vector<QPoint>(cols, QPoint(-1, -1)));
    std::queue<QPoint> open;

    // The queue expands outward from the enemy one tile at a time.
    visited[startRow][startCol] = true;
    open.push(QPoint(startCol, startRow));

    const QPoint dirs[] = {
        QPoint(1, 0),
        QPoint(-1, 0),
        QPoint(0, 1),
        QPoint(0, -1),
    };

    bool found = false;
    while (!open.empty() && !found)
    {
        QPoint current = open.front();
        open.pop();

        if (current.x() == goalCol && current.y() == goalRow)
        {
            found = true;
            break;
        }

        for (const QPoint &dir : dirs)
        {
            int nextCol = current.x() + dir.x();
            int nextRow = current.y() + dir.y();

            if (nextRow < 0 || nextRow >= rows || nextCol < 0 || nextCol >= cols)
                continue;
            if (visited[nextRow][nextCol])
                continue;
            if ((nextRow != goalRow || nextCol != goalCol) && pathTileBlocked(map, nextRow, nextCol))
                continue;

            // here we remember where this tile came from so we can walk the path backward later.
            visited[nextRow][nextCol] = true;
            parent[nextRow][nextCol] = current;
            open.push(QPoint(nextCol, nextRow));
        }
    }

    if (!found)
        return false;

    QPoint step(goalCol, goalRow);
    QPoint previous = parent[step.y()][step.x()];

    // We only need the first step, not the whole route, because the enemy recalculates often.
    while (previous != QPoint(-1, -1) && previous != QPoint(startCol, startRow))
    {
        step = previous;
        previous = parent[step.y()][step.x()];
    }

    nextPoint = tileCenter(step.y(), step.x(), tileSize);
    return true;
}
}

BaseEnemy::BaseEnemy(int hp, int atk, int def, float spd, float range, std::string damagePath)
{ // here is the constructor for each enemy

    health = hp;
    maxHealth = hp;
    attack = atk;
    defense = def;
    speed = spd;
    Dir.x = 0;
    Dir.y = 0;
    attackRange = range;
    damageSound = damagePath;
    // made timer for the animations inorder to run the update function each 100ms
    aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &BaseEnemy::update);
    aiTimer->start(100); //
}
// here i am just setting player to be able to connect both for attacking
void BaseEnemy::setPlayer(Player *p)
{
    player = p;
}
void BaseEnemy::detectandmove(Player *player)
{ // finding the difference in distance between player and
    // enemy so if less than attack sets movement direction
    // Use the visible centers for path choice so enemies chase the player instead of a sprite corner.
    float diffX = player->sceneBoundingRect().center().x() - this->collisionHitbox().center().x();
    float diffY = player->sceneBoundingRect().center().y() - this->collisionHitbox().center().y();

    float distance = sqrt(diffX * diffX + diffY * diffY);

    if (distance < 150)
    {
        QPointF targetPoint;
        MapLoader *map = dynamic_cast<MapLoader *>(scene());

        if (map && nextPathPoint(map, collisionHitbox().center(), player->sceneBoundingRect().center(), targetPoint))
        {
            // If a route exists, steer toward the next tile in the route instead of the player directly.
            diffX = targetPoint.x() - collisionHitbox().center().x();
            diffY = targetPoint.y() - collisionHitbox().center().y();
            distance = sqrt(diffX * diffX + diffY * diffY);
        }

        if (distance > 0)
        {
            Dir.x = diffX / distance;
            Dir.y = diffY / distance;
        }
        else
        {
            Dir.x = 0;
            Dir.y = 0;
        }
    }
    else
    {
        //  stop moving
        Dir.x = 0;
        Dir.y = 0;
    }
}
bool BaseEnemy::isalive()
{
    return (health > 0);
}

bool BaseEnemy::isdead()
{
    return (health <= 0);
}

void BaseEnemy::TakeDamage(int amount)
{ // if enemy is already dead or in hurt process dont take damage and return
    if (currentState == EnemyState::Dead || currentState == EnemyState::Hurt)
        return;
    // otherwise
    health -= amount;
    emit healthChanged(health > 0 ? health : 0, maxHealth);

    if (health > 0)
    {
        currentState = EnemyState::Hurt;
        currentFrame = 0;
        waitCounter = hurtData.frameCount; //  match animation length so ensuring it doesnt move until hurt finishes
        AudioManager::instance().playSound(damageSound);
        aiTimer->setInterval(70); // making the attack animation faster
    }
    else
    {
        health = 0;
        currentState = EnemyState::Dead;
        currentFrame = 0;
    }
}

void BaseEnemy::updateAnimation()
{ // here just cropping and frames correctly from spritesheets depending on enemystate
    AnimData *currentData = nullptr;
    QPixmap *currentSheet = nullptr;

    if (currentState == EnemyState::Idle)
    {
        currentData = &idleData;
        currentSheet = &idleSheet;
    }
    else if (currentState == EnemyState::Walking)
    {
        currentData = &walkData;
        currentSheet = &walkSheet;
    }
    else if (currentState == EnemyState::Attacking)
    {
        currentData = &attackData;
        currentSheet = &attackSheet;
    }
    else if (currentState == EnemyState::Hurt)
    {
        currentData = &hurtData;
        currentSheet = &hurtSheet;
    }
    else if (currentState == EnemyState::Dead)
    {
        currentData = &deadData;
        currentSheet = &deadSheet;
    }

    int frameW = currentData->frameWidth;
    int frameH = currentData->frameHeight;
    int xCrop = currentFrame * frameW;
    int yCrop = currentRow * frameH;

    this->setPixmap(currentSheet->copy(xCrop, yCrop, frameW, frameH));

    //  Only loop if NOT dead. If dead, stay on the last frame.
    if (currentState == EnemyState::Dead)
    {
        if (currentFrame < currentData->frameCount - 1)
        {
            currentFrame++;
        }
    }
    else
    { // Normal states loop their animations
        currentFrame = (currentFrame + 1) % currentData->frameCount;
    }
}

void BaseEnemy::moveEnemy()
{ // move enemy logic
    float currentSpeed = speed;

    float newX = this->x() + (Dir.x * currentSpeed);
    float newY = this->y() + (Dir.y * currentSpeed);

    MapLoader *map = dynamic_cast<MapLoader *>(scene());
    if (map)
    {
        int tileSize = map->tileSize();
        int fw = this->pixmap().width();
        int fh = this->pixmap().height();

        // Check X and Y separately so enemies can slide along objects instead of freezing.
        QRectF hitboxX(newX, this->y() + fh * 0.6f, fw, fh * 0.4f);
        int lCol = (int)std::floor(hitboxX.left()   / tileSize);
        int rCol = (int)std::floor(hitboxX.right()  / tileSize);
        int tRow = (int)std::floor(hitboxX.top()    / tileSize);
        int bRow = (int)std::floor(hitboxX.bottom() / tileSize);
        const auto &objects = map->getActiveCollidables();

        bool blockedX = false;
        for (int r = tRow; r <= bRow && !blockedX; r++)
            for (int c = lCol; c <= rCol && !blockedX; c++)
                if (map->isTileCollidable(r, c)) blockedX = true;
        if (!blockedX)
            for (const auto &obj : objects)
                if (hitboxX.intersects(obj.worldHitbox)) { blockedX = true; break; }
        if (!blockedX) this->setX(newX);

        QRectF hitboxY(this->x(), newY + fh * 0.6f, fw, fh * 0.4f);
        lCol = (int)std::floor(hitboxY.left()   / tileSize);
        rCol = (int)std::floor(hitboxY.right()  / tileSize);
        tRow = (int)std::floor(hitboxY.top()    / tileSize);
        bRow = (int)std::floor(hitboxY.bottom() / tileSize);
        bool blockedY = false;
        for (int r = tRow; r <= bRow && !blockedY; r++)
            for (int c = lCol; c <= rCol && !blockedY; c++)
                if (map->isTileCollidable(r, c)) blockedY = true;
        if (!blockedY)
            for (const auto &obj : objects)
                if (hitboxY.intersects(obj.worldHitbox)) { blockedY = true; break; }
        if (!blockedY) this->setY(newY);
    }
    else
    {
        this->setPos(newX, newY);
    }

    this->setZValue(this->y() + this->pixmap().height());
}

QRectF BaseEnemy::collisionHitbox() const
{
    // Only the lower body collides; the top of the sprite can overlap scenery naturally.
    QRectF body = sceneBoundingRect();
    return QRectF(body.left(),
                  body.top() + body.height() * 0.60,
                  body.width(),
                  body.height() * 0.40);
}

void BaseEnemy::update()
{
    if (!scene())
        return;

    if (scene()->views().isEmpty())
    {
        return;
    }

    if (!player)
        return;

    if (currentState == EnemyState::Dead)
    {
        updateAnimation();
        // Check if we have reached the final frame of the death sheet
        if (currentFrame >= deadData.frameCount - 1)
        {
            aiTimer->stop();
            this->hide();
            this->setEnabled(false);

            // 30% chance for a heart to be dropped
            if (QRandomGenerator::global()->bounded(100) < 30)
            {
                float centerX = this->x() + (this->boundingRect().width() / 2.0f);
                float centerY = this->y() + (this->boundingRect().height() / 2.0f);
                Heart *droppedHeart = new Heart(centerX, centerY, player);
                this->scene()->addItem(droppedHeart);
            }
            emit enemyDied();
        }
        return;
    }

    if (currentState == EnemyState::Hurt)
    {
        updateAnimation();
        waitCounter--;
        // once finish hurt we go back to idle state
        if (waitCounter <= 0)
        {
            currentState = EnemyState::Idle;
            currentFrame = 0;
        }
        return;
    }

    EnemyState previousState = currentState;

    float dx = player->x() - x();
    float dy = player->y() - y();
    float distance = sqrt(dx * dx + dy * dy);

    // Update currentRow based on player position
    if (distance < 150)
    {
        if (abs(dx) > abs(dy))
        {
            currentRow = (dx > 0) ? 3 : 2;
        }
        else
        {
            currentRow = (dy > 0) ? 0 : 1;
        }
    }

    // enemy doesn't move while attacking and so attack and hurt animations
    // cycle faster than walking and idle
    if (currentState == EnemyState::Attacking)
    {
        Dir.x = 0;
        Dir.y = 0;

        if (currentFrame == 6)
        {
            if (distance <= attackRange)
            {
                player->takeDamage(attack);
            }
        }

        if (currentFrame == 0 && previousState == EnemyState::Attacking)
        {
            currentState = EnemyState::Idle;
            waitCounter = 10; // Cooldown duration
            aiTimer->setInterval(100);
        }
    }
    else if (distance <= attackRange)
    {
        // stop when player in range
        Dir.x = 0;
        Dir.y = 0;

        // delay to give player time to dodge
        if (waitCounter > 0)
        {
            waitCounter--;
            currentState = EnemyState::Idle;
        }
        else
        {
            currentState = EnemyState::Attacking;
            currentFrame = 0;
            aiTimer->setInterval(70);
        }
    }
    else if (distance < 150)
    {
        currentState = EnemyState::Walking;

        if (waitCounter <= 0)
        {
            waitCounter = 5;
        }
    }
    else
    {
        currentState = EnemyState::Idle;
        Dir.x = 0;
        Dir.y = 0;
    }

    if (currentState != previousState)
    {
        currentFrame = 0;
    }

    if (currentState == EnemyState::Walking)
    {
        detectandmove(player);
        moveEnemy();
    }

    updateAnimation();
}
