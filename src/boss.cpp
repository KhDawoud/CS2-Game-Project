#include "boss.hpp"
#include <cmath>
#include <QRandomGenerator>
#include <QTimer>

Boss::Boss(int variant)
    : BaseEnemy(
          300,           // health
          15,            // attack
          10,            // defense
          2.5f,          // speed
          40.0f,         // range
          "BossDamage"), // sound
      currentPhase(BossPhase::IdleWait),
      phaseTimer(0),
      chargeCounter(0),
      chargeSpeedMultiplier(2.5f) // how fast the boss charges
{
    loadAnimations();

    currentState = EnemyState::Idle;

    this->setScale(2.0);
}

void Boss::loadAnimations()
{
    QString path = ":/resources/boss/";

    idleSheet.load(path + "boss_idle.png");
    walkSheet.load(path + "boss_walk.png");
    attackSheet.load(path + "boss_attack.png");
    deadSheet.load(path + "boss_death.png");
    hurtSheet.load(path + "boss_hurt.png");

    idleData = {idleSheet, 4, 100, 64, 64};
    walkData = {walkSheet, 6, 100, 64, 64};
    attackData = {attackSheet, 8, 100, 64, 64};
    deadData = {deadSheet, 8, 100, 64, 64};
    hurtData = {hurtSheet, 4, 100, 64, 64};
}

void Boss::update()
{
    if (!this->scene() || !player || !player->scene() || this->scene() != player->scene())
    {
        return;
    }

    if (currentState == EnemyState::Dead)
    {
        updateAnimation();
        if (currentFrame >= deadData.frameCount - 1)
        {
            aiTimer->stop();
            hide();
            setEnabled(false);
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
            aiTimer->setInterval(100);
        }
        return;
    }

    // get distance and angle to player
    float diffX = player->x() - this->x();
    float diffY = player->y() - this->y();
    float distance = std::sqrt(diffX * diffX + diffY * diffY);

    if (distance > 0)
    {
        Dir.x = diffX / distance;
        Dir.y = diffY / distance;
    }

    if (std::abs(Dir.x) > std::abs(Dir.y))
    {
        currentRow = (Dir.x > 0) ? 3 : 2; // 3 = Right, 2 = Left
    }
    else
    {
        currentRow = (Dir.y > 0) ? 0 : 1; // 0 = Down,  1 = Up
    }

    switch (currentPhase)
    {
    case BossPhase::IdleWait:
        currentState = EnemyState::Idle;
        phaseTimer++;

        if (phaseTimer >= 40)
        {
            currentPhase = BossPhase::Charging;
            phaseTimer = 0;
            chargeCounter = 0;
            aiTimer->setInterval(40);
        }
        break;

    case BossPhase::Charging:
        currentState = EnemyState::Walking;

        this->setPos(x() + Dir.x * (speed * chargeSpeedMultiplier),
                     y() + Dir.y * (speed * chargeSpeedMultiplier));

        if (distance <= attackRange)
        {
            currentPhase = BossPhase::Swing1;
            currentState = EnemyState::Attacking;
            phaseTimer = 0;
            currentFrame = 0;

            aiTimer->setInterval(100);
        }
        break;

    case BossPhase::Swing1:
        currentState = EnemyState::Attacking;

        if (currentFrame == 3 && phaseTimer == 0)
        {
            if (distance <= attackRange)
            {
                player->takeDamage(attack);
            }
            phaseTimer = 1;
        }

        if (currentFrame >= attackData.frameCount - 1)
        {
            currentPhase = BossPhase::ShortDelay;
            currentState = EnemyState::Idle;
            phaseTimer = 0;
            currentFrame = 0;
        }
        break;

    case BossPhase::ShortDelay:
        currentState = EnemyState::Idle;
        phaseTimer++;

        if (phaseTimer >= 5)
        {
            currentPhase = BossPhase::Swing2;
            currentState = EnemyState::Attacking;
            phaseTimer = 0;
            currentFrame = 0;
        }
        break;

    case BossPhase::Swing2:
        currentState = EnemyState::Attacking;

        if (currentFrame == 3 && phaseTimer == 0)
        {
            if (distance <= attackRange)
            {
                player->takeDamage(attack);
            }
            phaseTimer = 1;
        }

        if (currentFrame >= attackData.frameCount - 1)
        {
            chargeCounter++;

            if (chargeCounter < 2)
            {
                currentPhase = BossPhase::Charging;
                aiTimer->setInterval(40);
            }
            else
            {
                currentPhase = BossPhase::IdleWait;
                aiTimer->setInterval(100);
            }
            phaseTimer = 0;
            currentFrame = 0;
        }
        break;
    }
    updateAnimation();
}
