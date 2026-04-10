#include "slime.hpp"

// Default slime is type 1
Slime::Slime()
    : BaseEnemy(50, 5, 2, 1.0f, 40.0f), slimenumber(1)
{
    loadAnimations();
}

// Constructor with slime number
Slime::Slime(int num)
    : BaseEnemy(
          (num == 1 ? 50 : (num == 2 ? 60 : 70)), // health
          (num == 1 ? 5  : (num == 2 ? 10 : 15)), // attack
          (num == 1 ? 2  : (num == 2 ? 4  : 8)),  // defense
          (num == 1 ? 1.0f : (num == 2 ? 2.0f : 3.0f)), // speed
          (num == 1 ? 20.0f : (num == 2 ? 20.0f : 20.0f)) // range
          ),
    slimenumber(num)
{
    loadAnimations();
}


void Slime::loadAnimations()
{
    QString path = ":/resources/enemy/";

    if (slimenumber == 1)
    {
        idleSheet.load(path + "idle/Slime1_Idle_with_shadow.png");
        walkSheet.load(path + "walk/Slime1_Walk_with_shadow.png");
        attackSheet.load(path + "attack/Slime1_Attack_with_shadow.png");
        deadSheet.load(path + "dead/Slime1_Death_with_shadow.png");
        hurtSheet.load(path + "hurt/Slime1_Hurt_with_shadow.png");

        idleData.frameCount = 6; idleData.frameWidth = 64; idleData.frameHeight = 64;
        walkData.frameCount = 8; walkData.frameWidth = 64; walkData.frameHeight = 64;
        attackData.frameCount = 10; attackData.frameWidth = 64; attackData.frameHeight = 64;
        deadData.frameCount = 10; deadData.frameWidth = 64; deadData.frameHeight = 64;
        hurtData.frameCount = 5; hurtData.frameWidth = 64; hurtData.frameHeight = 64;
    }
    else if (slimenumber == 2)
    {
        idleSheet.load(path + "idle/Slime2_Idle_with_shadow.png");
        walkSheet.load(path + "walk/Slime2_Walk_with_shadow.png");
        attackSheet.load(path + "attack/Slime2_Attack_with_shadow.png");
        deadSheet.load(path + "dead/Slime2_Death_with_shadow.png");
        hurtSheet.load(path + "hurt/Slime2_Hurt_with_shadow.png");

        idleData.frameCount = 6; idleData.frameWidth = 64; idleData.frameHeight = 64;
        walkData.frameCount = 8; walkData.frameWidth = 64; walkData.frameHeight = 64;
        attackData.frameCount = 11; attackData.frameWidth = 64; attackData.frameHeight = 64;
        deadData.frameCount = 10; deadData.frameWidth = 64; deadData.frameHeight = 64;
        hurtData.frameCount = 5; hurtData.frameWidth = 64; hurtData.frameHeight = 64;
    }
    else // slime 3
    {
        idleSheet.load(path + "idle/Slime3_Idle_with_shadow.png");
        walkSheet.load(path + "walk/Slime3_Walk_with_shadow.png");
        attackSheet.load(path + "attack/Slime3_Attack_with_shadow.png");
        deadSheet.load(path + "dead/Slime3_Death_with_shadow.png");
        hurtSheet.load(path + "hurt/Slime3_Hurt_with_shadow.png");

        idleData.frameCount = 6; idleData.frameWidth = 64; idleData.frameHeight = 64;
        walkData.frameCount = 8; walkData.frameWidth = 64; walkData.frameHeight = 64;
        attackData.frameCount = 9; attackData.frameWidth = 64; attackData.frameHeight = 64;
        deadData.frameCount = 10; deadData.frameWidth = 64; deadData.frameHeight = 64;
        hurtData.frameCount = 5; hurtData.frameWidth = 64; hurtData.frameHeight = 64;
    }
    if (idleSheet.isNull())
    {
        qDebug() << "Idle sheet is NULL → crash prevented";
        return;
    }
    setPixmap(idleSheet.copy(0, 0, idleData.frameWidth, idleData.frameHeight));
}
