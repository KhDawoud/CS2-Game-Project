#include "Enemy.hpp"
#include <QDebug>
#include <QTimer>
#include <QTransform>
#include "player.hpp"

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

    if (distance < 100) {
        if (diffX > 0)
            Dir.x = 1; // Player is to the right
        else if (diffX < 0)
            Dir.x = -1; // Player is to the left

        if (diffY > 0)
            Dir.y = 1; // Player is below
        else if (diffY < 0)
            Dir.y = -1;
    } else {
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

    if (health > 0) {
        currentState = EnemyState::Hurt;
        currentFrame = 0;
        waitCounter = hurtData.frameCount; // Changed to match animation length
    } else {
        health = 0;
        currentState = EnemyState::Dead;
        currentFrame = 0;
        emit enemyDied();
    }
}

void BaseEnemy::updateAnimation()
{
    AnimData *currentData = nullptr;
    QPixmap *currentSheet = nullptr;

    if (currentState == EnemyState::Idle) {
        currentData = &idleData;
        currentSheet = &idleSheet;
    } else if (currentState == EnemyState::Walking) {
        currentData = &walkData;
        currentSheet = &walkSheet;
    } else if (currentState == EnemyState::Attacking) {
        currentData = &attackData;
        currentSheet = &attackSheet;
    } else if (currentState == EnemyState::Hurt) {
        currentData = &hurtData;
        currentSheet = &hurtSheet;
    } else if (currentState == EnemyState::Dead) {
        currentData = &deadData;
        currentSheet = &deadSheet;
    }

    int frameW = currentData->frameWidth;
    int frameH = currentData->frameHeight;
    int xCrop = currentFrame * frameW;
    int yCrop = currentRow * frameH;

    this->setPixmap(currentSheet->copy(xCrop, yCrop, frameW, frameH));

    // Logic: Only loop if NOT dead. If dead, stay on the last frame.
    if (currentState == EnemyState::Dead) {
        if (currentFrame < currentData->frameCount - 1) {
            currentFrame++;
        }
    } else {
        currentFrame = (currentFrame + 1) % currentData->frameCount;
    }
}

void BaseEnemy::moveEnemy()
{
    float currentSpeed = speed;

    if (Dir.x != 0 && Dir.y != 0) {
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

    if (currentState == EnemyState::Dead) {
        updateAnimation();
        // Check if we have reached the final frame of the death sheet
        if (currentFrame >= deadData.frameCount - 1) {
            this->hide();
            this->setEnabled(false);
        }
        return;
    }

    if (currentState == EnemyState::Hurt) {
        updateAnimation();
        waitCounter--;

        if (waitCounter <= 0) {
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
    if (distance < 150) {
        if (abs(dx) > abs(dy)) {
            currentRow = (dx > 0) ? 3 : 2;
        } else {
            currentRow = (dy > 0) ? 0 : 1;
        }
    }

    // --- STATE LOGIC ---
    if (distance < attackRange)
    {
        if (currentState != EnemyState::Attacking)
        {
            if (waitCounter <= 0)
            {
                waitCounter = 20;
                currentState = EnemyState::Idle;
            }
            else
            {
                waitCounter--;

                if (waitCounter == 0)
                {
                    currentState = EnemyState::Attacking;
                    attackTimer = attackDuration;
                    currentFrame = 0;
                }
            }
        }
    }
    else if (distance < 150)
    {
        currentState = EnemyState::Walking;
    }
    else
    {
        currentState = EnemyState::Idle;
    }


    if (currentState == EnemyState::Attacking) {
        if (attackTimer > 0) {
            attackTimer--;
        } else {
            currentState = EnemyState::Idle;
            waitCounter = 10; // Cooldown
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
