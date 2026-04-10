#ifndef CHARACTERSTATS_HPP
#define CHARACTERSTATS_HPP
#include "player.hpp"
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPixmap>

class CharacterStats : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
private:
    float health;
    float mana;
    float stamina;
    Player *player = nullptr;
    QPixmap spriteSheet;

public slots:
    void updateBars();

public:
    CharacterStats();
    void setPlayer(Player *player);
    void updateValues();
};
#endif
