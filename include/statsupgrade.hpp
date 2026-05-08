#ifndef STATSUPGRADE_HPP
#define STATSUPGRADE_HPP
#include "basewindow.hpp"
#include "player.hpp"
class Statsupgrade: public BaseWindow
{
public:
    Statsupgrade(QWidget*, Stats, int);
};

#endif // STATSUPGRADE_HPP
