#include "slime.hpp"

Slime::Slime()
    : BaseEnemy(50, 5, 2, 1.5f, 5.0) // hp, atk, def, speed
{
    loadAnimations();
}
void Slime::loadAnimations()
{
    idleSheet.load(":resources/enemy/idle/Slime1_Idle_with_shadow.png");
    idleData.frameCount = 6;
    idleData.frameWidth = 64;
    idleData.frameHeight = 64;

    walkSheet.load(":resources/enemy/walk/Slime1_Walk_with_shadow.png");
    walkData.frameCount = 8;
    walkData.frameWidth = 64;
    walkData.frameHeight = 64;

    attackSheet.load(":resources/enemy/attack/Slime1_Attack_with_shadow.png");
    attackData.frameCount = 10;
    attackData.frameWidth = 64;
    attackData.frameHeight = 64;

    deadSheet.load(":resources/enemy/dead/Slime1_Death_with_shadow.png");
    deadData.frameCount = 10;
    deadData.frameWidth = 64;
    deadData.frameHeight = 64;

    hurtSheet.load(":resources/enemy/hurt/Slime1_Hurt_with_shadow.png");
    hurtData.frameCount = 5;
    hurtData.frameWidth = 64;
    hurtData.frameHeight = 64;
}
