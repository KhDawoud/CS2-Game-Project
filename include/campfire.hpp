#ifndef CAMPFIRE_HPP
#define CAMPFIRE_HPP
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QObject>
class Campfire: public QObject, public QGraphicsPixmapItem
{
     Q_OBJECT
private:
    QPixmap framesheet;
    int FrameWidth;
    const int framecount = 6;
    int currentframe;
    QTimer* animationTimer;
public:
    Campfire();
    void UpdateAnimation();
    int getwidth();
    int getheight();

};

#endif // CAMPFIRE_HPP
