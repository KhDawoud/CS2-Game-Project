#include "Enemy.hpp"
#include "player.hpp"
#include <QTimer>
#include <QTransform>


using namespace std;

BaseEnemy::BaseEnemy(int hp, int atk, int def, float spd)
{
    health = hp;
    maxHealth = hp;
    attack = atk;
    defense = def;
    speed = spd;

    QTimer *aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &BaseEnemy::update);
    aiTimer->start(100); // Ticks every 0.1 seconds
}

void BaseEnemy::detectandmove(Player *player)
{
    float diffX = player->x() - this->x();
    float diffY = player->y() - this->y();

    // 2. Check if the player is within 100  pixels
    if (abs(diffX) < 100 && abs(diffY) < 100) {
        if (diffX > 0)
            Dir.x = 1; // Player is to the right
        else if (diffX < 0)
            Dir.x = -1; // Player is to the left

        if (diffY > 0)
            Dir.y = 1; // Player is below
        else if (diffY < 0)
            Dir.y = -1; // Player is above

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

    void BaseEnemy::attack(Player & target)
    {
        int damage = attack - target.defense;
        if (damage < 1)
            damage = 1;

        target.TakeDamage(damage);
    }

    void BaseEnemy::TakeDamage(int amount)
    {
        health -= amount;
        if (health < 0)
            health = 0;
    }

    void BaseEnemy::updateAnimation()
    {
        AnimData *currentData = nullptr; //declaring pointer to point to nothing

        if (currentState == EnemyState::Idle) {
            currentData = &idleData;
        }

        else if (currentState == EnemyState::Walking) {
            currentData = &walkData;
        } else if (currentState == EnemyState::Attacking) {
            currentData = &attackData;
        } //deciding which animation to take using switch case

        if (Dir.y < 0) {
            currentRow = 1;
        }

        else if (Dir.x > 0) {
            currentRow = 3;
        } else if (Dir.x < 0) {
            currentRow = 2;
        } else {
            currentRow = 0;
        }

        int xCrop = currentFrame * width; // horizontal starting point of each frame
        int yCrop = currentRow * height;  // which row we are in

        this->setPixmap(
            spriteSheet.copy(xCrop, yCrop, width, height)); // take frames and show in screen
        currentFrame = (currentFrame + 1) % totalFrames; // looping across frame to show animation
    }

    void BaseEnemy::moveEnemy()
    {
        float currentSpeed = speed; //to set diagonal

        // If moving diagonally
        if (Dir.x != 0 && Dir.y != 0) {
            currentSpeed = speed * 0.707f;

            // Calculate new position
            float newX = this->x() + (Dir.x * currentspeed);
            float newY = this->y() + (Dir.y * currentspeed);

            // Update the position
            this->setPos(newX, newY);
        }
    }

        void BaseEnemy::update()
        {
            if (!isalive()) {
                // If the enemy is dead, don't move or animate
                return;
            }
            detectAndMove(player);
            moveEnemy();       // 1. Change the X,y
            updateAnimation(); // 2. Change the picture to match movement
        }
    }
