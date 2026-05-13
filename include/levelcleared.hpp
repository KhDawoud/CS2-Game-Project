#ifndef LEVELCLEARED_H
#define LEVELCLEARED_H
#include "basewindow.hpp"
class LevelCleared : public BaseWindow
{
public:
    LevelCleared(QWidget *,int);
private:
    int CurrentLevel;
    bool savedAndQuit = false;   // true when player chose Save & Quit
};

#endif // LEVELCLEARED_H
