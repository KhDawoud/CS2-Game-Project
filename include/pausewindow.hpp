#ifndef PAUSEWINDOW_H
#define PAUSEWINDOW_H
#include "basewindow.hpp"

class pausewindow: public BaseWindow
{
public:
    // inLevel = true shows the "Save & Quit" button; false (interior) hides it
    explicit pausewindow(QWidget *parent, bool inLevel = false);

    bool savedAndQuit = false;   // true when player chose Save & Quit
};

#endif // PAUSEWINDOW_H
