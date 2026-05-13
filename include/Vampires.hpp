#ifndef VAMPIRES_HPP
#define VAMPIRES_HPP
#include "Enemy.hpp"
#include <QPixmap>

class Vampire : public BaseEnemy
{
public:
    Vampire();
    int vampirenumber;
    void loadAnimations();
    Vampire(int num);
};

#endif // VAMPIRES_HPP
