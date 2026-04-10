#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP
#include <QGraphicsView>
#include "player.hpp"
#include "map.hpp"
#include "house_interior.hpp"


class GameView: public QGraphicsView
{
    Q_OBJECT
public:
    GameView(Map*, House_Interior*, Player*);
    void keyPressEvent(QKeyEvent *event) override;
signals:
    void isoverworld(bool);

private:
    Map* _overworld;
    House_Interior* _interior;
    Player* _player;
};


#endif // GAMEVIEW_HPP
