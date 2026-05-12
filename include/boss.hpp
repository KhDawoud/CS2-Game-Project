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
    Swing2
};

class Boss : public BaseEnemy
{
    Q_OBJECT

public:
    Boss(int variant = 1);
    void loadAnimations();

public slots:
    void update() override;

private:
    BossPhase currentPhase;
    int phaseTimer;
    int chargeCounter; // how many times we do the combo

    float chargeSpeedMultiplier;
};

#endif // BOSS_HPP