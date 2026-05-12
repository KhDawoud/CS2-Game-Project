#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP

#include <QGraphicsView>
#include <QProgressBar>
#include <QLabel>
#include "maploader.hpp"
#include "characters.hpp"

class GameView : public QGraphicsView
{
    Q_OBJECT

public:
    GameView(MapLoader *overworld, MapLoader *interior, MapLoader *level2, MapLoader *level3, Characters *player);
    void checkInteractions();
    QProgressBar *_bossHealthBar;
    QLabel *_bossLabel;
    QProgressBar *_progressBar;

signals:
    void isoverworld(bool toggle);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

private:
    void switchToInterior();
    void switchToCharacterSelectScreen();
    void switchToOverworld();
    void switchtoLevel2();
    void switchtoLevel3();

    // these handle the torch effect
    qreal _currentLightRadius = 120.0;
    qreal _flickerPhase = 0.0;
    QTimer *_flickerTimer;

    MapLoader *_overworld;
    MapLoader *_interior;
    MapLoader *Level2;
    MapLoader *Level3;
    Characters *_player;

    QGraphicsTextItem *textStart;
    QGraphicsPixmapItem *interactPrompt;
    QGraphicsTextItem *textEnd;
    void loadinteractionPrompt();
    QString pixelFontFamily;
    float currentscale = 1.2;
};

#endif // GAMEVIEW_HPP
