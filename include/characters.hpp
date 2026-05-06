#ifndef CHARACTERS_HPP
#define CHARACTERS_HPP
#include "player.hpp"

class Characters : public Player
{
private:
    void loadSwordsman();
    void loadWizard();
    void loadVampire();
public:
    Characters(int);
    void swtichto(int);


};

#endif // CHARACTERS_HPP
