#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QGraphicsScene>
#include "Enemy.hpp"

class MapLoader;

class Projectile : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Projectile(QPointF startPos, QPointF direction, MapLoader *map);

private slots:
    void move();

private:
    QPixmap sheet;
    QPointF velocity;
    qreal speed = 2.0;
    int currentFrame = 0;
    int maxFrames = 5;
    QPointF startPoint;
    qreal maxRange = 100.0;
    MapLoader *currentMap;
    bool isExploding = false;
    int explosionFrame = 0;
    int explosionCounter = 0;
    QPixmap explosionSheet;

    void startExplosion();
    void updateAnimation();
};

#endif // PROJECTILE_HPP
