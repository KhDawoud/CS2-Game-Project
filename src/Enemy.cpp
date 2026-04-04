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
    
    if (Dir.y < 0)
    {
        currentRow = 1; // up
    }
    else if (Dir.x > 0)
    {
        currentRow = 3; // right
    }
    else if (Dir.x < 0)
    {
        currentRow = 2; // left
    }
    else
    {
        currentRow = 0; // down
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
    if (!isalive()) return;
    if (!player) return;

    EnemyState previousState = currentState;

    float dx = player->x() - x();
    float dy = player->y() - y();
    float distance = sqrt(dx * dx + dy * dy);

    // --- STATE LOGIC ---

    
    if (currentState == EnemyState::Walking && distance < attackRange) {
        currentState = EnemyState::Idle;
        waitCounter = 10; // 3 seconds (30 * 0.1s)
    }

   
    else if (currentState == EnemyState::Idle) {

        if (waitCounter > 0) {
            waitCounter--;
        }
        else {
            // Timer finished
            if (distance < attackRange) {
                currentState = EnemyState::Attacking;
                attackTimer = attackDuration; // start attack
            } else {
                currentState = EnemyState::Walking;
            }
        }
    }

    
    else if (currentState == EnemyState::Attacking) {

        if (attackTimer > 0) {
            attackTimer--;
        }
        else {
            currentState = EnemyState::Idle;
            waitCounter = 10; // cooldown after attack
        }
    }

  
    if (currentState != previousState) {
        currentFrame = 0;
    }

   
    if (currentState == EnemyState::Walking) {
        detectandmove(player);
        moveEnemy();
    } else {
        
        Dir.x = 0;
        Dir.y = 0;
    }

    
    updateAnimation();
}

