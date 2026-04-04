#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP
#include <QGraphicsView>
#include "player.hpp"
#include "map.hpp"


class GameView: public QGraphicsView
{
public:
    GameView(Map*, Player*);
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // GAMEVIEW_HPP
