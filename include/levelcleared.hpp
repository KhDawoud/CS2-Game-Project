#ifndef LEVELCLEARED_H
#define LEVELCLEARED_H
#include "basewindow.hpp"
class LevelCleared : public BaseWindow
{
public:
    LevelCleared(QWidget *,int);
protected:
    int CurrentLevel;
};

#endif // LEVELCLEARED_H
