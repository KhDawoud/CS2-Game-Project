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
    GameView(MapLoader *overworld, MapLoader *interior, MapLoader* level3,Characters *player);
    void checkInteractions();

signals:
    void isoverworld(bool toggle);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void switchToInterior();
    void switchToOverworld();
    void switchtoLevel3();
    MapLoader *_overworld;
    MapLoader *_interior;
    MapLoader *Level3;
    Characters *_player;

    QProgressBar *_progressBar;
    QGraphicsTextItem* textStart;
    QGraphicsPixmapItem* interactPrompt;
    QGraphicsTextItem* textEnd;
    void loadinteractionPrompt();
    QString pixelFontFamily;
};

#endif // GAMEVIEW_HPP
