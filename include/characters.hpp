#ifndef CHARACTERS_HPP
#define CHARACTERS_HPP
#include "player.hpp"

class Characters : public Player
{
private:
    void loadSwordsman();
    void loadWizard();
    void loadSkeleton();
public:
    Characters(int);
    void swtichto(int);
public slots:
    void handleLevelCleared();

};

#endif // CHARACTERS_HPP
