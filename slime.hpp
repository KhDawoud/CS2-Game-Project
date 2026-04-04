#ifndef SLIME_HPP
#define SLIME_HPP
#include "Enemy.hpp"
#include <QPixmap>

class Slime : public BaseEnemy
{
public:
    Slime();

    void loadAnimations();
};

#endif // SLIME_HPP
