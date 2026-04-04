#include "Enemy.hpp"
#include "player.hpp"
#include <QTimer>
#include <QTransform>

using namespace std;

BaseEnemy::BaseEnemy(int hp, int atk, int def, float spd, float range)
{
    health = hp;
    maxHealth = hp;
    attack = atk;
    defense = def;
    speed = spd;
    Dir.x = 0;
    Dir.y = 0;
    attackRange = range;
    QTimer *aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &BaseEnemy::update);
    aiTimer->start(100); // Ticks every 0.1 seconds
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
    health -= amount;
    if (health < 0)
        health = 0;
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

    // Determine which row of the sprite sheet to use based on direction
    if (Dir.y < 0)
    {
        currentRow = 1; // Up
    }
    else if (Dir.x > 0)
    {
        currentRow = 3; // Right
    }
    else if (Dir.x < 0)
    {
        currentRow = 2; // Left
    }
    else
    {
        currentRow = 0; // Down / Default
    }

    int frameW = currentData->frameWidth;
    int frameH = currentData->frameHeight;

    int xCrop = currentFrame * frameW;
    int yCrop = currentRow * frameH;

    this->setPixmap(currentSheet->copy(xCrop, yCrop, frameW, frameH));
    currentFrame = (currentFrame + 1) % currentData->frameCount;
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
    if (!isalive())
    {
        return;
    }
    detectandmove(player);

    EnemyState previousState = currentState;

    if (Dir.x == 0 && Dir.y == 0)
    {
        currentState = EnemyState::Idle;
    }
    else
    {
        currentState = EnemyState::Walking;
    }

    // Check if player is close enough to attack
    float distance = sqrt(pow(player->x() - x(), 2) + pow(player->y() - y(), 2));
    if (distance < attackRange)
    {
        currentState = EnemyState::Attacking;
    }

    // here we make sure if we're switching states that we start from 0
    if (currentState != previousState)
    {
        currentFrame = 0;
    }

    moveEnemy();
    updateAnimation();
}
