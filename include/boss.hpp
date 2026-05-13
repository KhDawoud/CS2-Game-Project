#ifndef BOSS_HPP
#define BOSS_HPP

#include "Enemy.hpp"
#include <QPixmap>

enum class BossPhase
{
    IdleWait,
    Charging,
    Swing1,
    ShortDelay,
    Swing2,
    FireballWindup,
    FireballRelease
};

class Boss : public BaseEnemy
{
    Q_OBJECT

public:
    Boss(int variant = 1);
    void loadAnimations();
    QRectF collisionHitbox() const override;

public slots:
    void update() override;

private:
    enum class FireballWallSide
    {
        Left,
        Right,
        Top,
        Bottom
    };

    BossPhase currentPhase;
    int phaseTimer;
    int chargeCounter; // how many times we do the combo
    bool nextAttackIsFireball;
    bool fireballsReleased;

    float chargeSpeedMultiplier;

    void facePlayer(float distance);
    void enterIdleWait();
    void beginNextAttack();
    void beginChargeAttack();
    void beginFireballAttack();
    void spawnFireballWall(FireballWallSide side);
    void releaseFireballs();
    void updateDepth();
    void updateIdleWait();
    void updateCharging(float distance);
    void updateSwing1(float distance);
    void updateShortDelay();
    void updateSwing2(float distance);
    void updateFireballWindup();
    void updateFireballRelease();
};

#endif // BOSS_HPP
