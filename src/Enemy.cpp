#include "Enemy.hpp"
#include <QDebug>
#include <QTimer>
#include <QTransform>
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <string>
#include "player.hpp"
#include "AudioManager.hpp"
#include "Heart.hpp"

using namespace std;

BaseEnemy::BaseEnemy(int hp, int atk, int def, float spd, float range, std::string damagePath)
{
    health = hp;
    maxHealth = hp;
    attack = atk;
    defense = def;
    speed = spd;
    Dir.x = 0;
    Dir.y = 0;
    attackRange = range;
    damageSound = damagePath;

    aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &BaseEnemy::update);
    aiTimer->start(100); //
}
void BaseEnemy::setPlayer(Player *p)
{
    player = p;
}
void BaseEnemy::detectandmove(Player *player)
{
    float diffX = player->x() - this->x();
    float diffY = player->y() - this->y();

    float distance = sqrt(diffX * diffX + diffY * diffY);

    if (distance < 100)
    {
        if (diffX > 0)
            Dir.x = 1; // Player is to the right
        else if (diffX < 0)
            Dir.x = -1; // Player is to the left

        if (diffY > 0)
            Dir.y = 1; // Player is below
        else if (diffY < 0)
            Dir.y = -1;
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
{
    if (currentState == EnemyState::Dead || currentState == EnemyState::Hurt)
        return;

    health -= amount;

    if (health > 0)
    {
        currentState = EnemyState::Hurt;
        currentFrame = 0;
        waitCounter = hurtData.frameCount; // Changed to match animation length
        AudioManager::instance().playSound(damageSound);
        aiTimer->setInterval(70); // making the attack animation a bit faster
    }
    else
    {
        health = 0;
        currentState = EnemyState::Dead;
        currentFrame = 0;
    }
}

void BaseEnemy::updateAnimation()
{
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

    // Logic: Only loop if NOT dead. If dead, stay on the last frame.
    if (currentState == EnemyState::Dead)
    {
        if (currentFrame < currentData->frameCount - 1)
        {
            currentFrame++;
        }
    }
    else
    {
        currentFrame = (currentFrame + 1) % currentData->frameCount;
    }
}

void BaseEnemy::moveEnemy()
{
    float currentSpeed = speed;

    if (Dir.x != 0 && Dir.y != 0)
    {
        currentSpeed = speed * 0.707f;
    }

    float newX = this->x() + (Dir.x * currentSpeed);
    float newY = this->y() + (Dir.y * currentSpeed);

    this->setPos(newX, newY);
}
void BaseEnemy::update()
{
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

    // changed it a bit so it doesn't move while attacking and so attack and hurt animations
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
