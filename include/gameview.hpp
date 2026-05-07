#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP

#include <QGraphicsView>
#include <QProgressBar>
#include "maploader.hpp"
#include "characters.hpp"

class GameView : public QGraphicsView
{
    Q_OBJECT

public:
    GameView(MapLoader *overworld, MapLoader *interior, Characters *player);

signals:
    void isoverworld(bool toggle);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void switchToInterior();
    void switchToOverworld();

    MapLoader *_overworld;
    MapLoader *_interior;
    Characters *_player;

    QProgressBar *_progressBar; 
};

#endif // GAMEVIEW_HPP