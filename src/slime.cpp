#include "slime.hpp"

Slime::Slime() : BaseEnemy(50, 5, 2, 1.5f, 5.0) // hp, atk, def, speed
{
    loadAnimations();
}
void Slime::loadAnimations()
{
    idleSheet.load(":resources/enemy/idle/Slime1_Idle_with_shadow.png");
    walkSheet.load(":resources/enemy/walk/Slime1_Walk_with_shadow.png");
    attackSheet.load(":resources/enemy/attack/Slime1_Attack_with_shadow.png");

    idleData.frameCount = 6;
    idleData.frameWidth = 64;
    idleData.frameHeight = 64;
    walkData.frameCount = 8;
    walkData.frameWidth = 64;
    walkData.frameHeight = 64;
    attackData.frameCount = 10;
    attackData.frameWidth = 64;
    attackData.frameHeight = 64;
}
