#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QGraphicsScene>
#include "Enemy.hpp"


class Projectile : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    Projectile(QPointF startPos, QPointF direction, Map* Map1, House_Interior* Map2);

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
    Map* gamemap;
    House_Interior* housemap;

};

#endif // PROJECTILE_HPP
