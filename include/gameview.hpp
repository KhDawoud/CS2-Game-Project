#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP
#include <QGraphicsView>
#include "characters.hpp"
#include "map.hpp"
#include "house_interior-2.hpp"


class GameView: public QGraphicsView
{
    Q_OBJECT
public:
    GameView(Map*, House_Interior*, Characters*);
    void keyPressEvent(QKeyEvent *event) override;
signals:
    void isoverworld(bool);

private:
    Map* _overworld;
    House_Interior* _interior;
    Characters* _player;
};


#endif // GAMEVIEW_HPP
