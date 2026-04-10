#ifndef SLIME_HPP
#define SLIME_HPP
#include "Enemy.hpp"
#include <QPixmap>

class Slime : public BaseEnemy
{
public:
    Slime();
    int slimenumber;
    void loadAnimations();
    Slime(int num);
};

#endif // SLIME_HPP
