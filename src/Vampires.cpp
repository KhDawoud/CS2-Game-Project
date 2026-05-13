#include "Vampires.hpp"
#include <QDebug>

Vampire::Vampire()
    : BaseEnemy(80, 15, 5, 2.5f, 30.0f, "SlimeDamage"), vampirenumber(1)
{
    loadAnimations();
}

Vampire::Vampire(int num)
    : BaseEnemy(
          (num == 1 ? 80  : (num == 2 ? 90 : 110)),
          (num == 1 ? 20  : (num == 2 ? 25  : 35)),
          (num == 1 ? 5   : (num == 2 ? 8   : 12)),
          (num == 1 ? 5.0f : (num == 2 ? 6.0f : 7.0f)),
          (num == 1 ? 30.0f : (num == 2 ? 32.0f : 35.0f)),
          "SlimeDamage"),
      vampirenumber(num)
{
    loadAnimations();
}

void Vampire::loadAnimations()
{
    QString n = QString::number(vampirenumber);
    QString path = ":/resources/vampires/vampire " + n + "/Vampires" + n;

    idleSheet.load(path   + "_Idle_with_shadow.png");
    qDebug() << "[Vampire] loading variant" << vampirenumber << "| idle null?" << idleSheet.isNull() << "| path:" << path + "_Idle_with_shadow.png";
    walkSheet.load(path   + "_Run_with_shadow.png");
    attackSheet.load(path + "_Attack_with_shadow.png");
    hurtSheet.load(path   + "_Hurt_with_shadow.png");
    deadSheet.load(path   + "_Death_with_shadow.png");

    // check BEFORE calculating dimensions — if null, width() returns 0
    if (idleSheet.isNull())
    {
        qDebug() << "Vampire: idle sheet failed to load for variant" << vampirenumber
                 << "| path tried:" << path + "_Idle_with_shadow.png";
        return;
    }

    idleData.frameCount   = 4;
    idleData.frameWidth   = idleSheet.width()   / 4;
    idleData.frameHeight  = idleSheet.height()  / 4;

    walkData.frameCount   = 8;
    walkData.frameWidth   = walkSheet.width()   / 8;
    walkData.frameHeight  = walkSheet.height()  / 4;

    attackData.frameCount = 12;
    attackData.frameWidth = attackSheet.width() / 12;
    attackData.frameHeight= attackSheet.height()/ 4;

    hurtData.frameCount   = 4;
    hurtData.frameWidth   = hurtSheet.width()   / 4;
    hurtData.frameHeight  = hurtSheet.height()  / 4;

    deadData.frameCount   = 11;
    deadData.frameWidth   = deadSheet.width()   / 11;
    deadData.frameHeight  = deadSheet.height()  / 4;

    setPixmap(idleSheet.copy(0, 0, idleData.frameWidth, idleData.frameHeight));
}
