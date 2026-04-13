#ifndef HEART_HPP
#define HEART_HPP

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>

class Player;

class Heart : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Heart(float x, float y, Player *p, QGraphicsItem *parent = nullptr);
    ~Heart(); // it'll despawn so it needs destructor

private slots:
    void updateTick();
    void despawn();

private:
    Player *player;
    QTimer *updateTimer;
    QTimer *lifespanTimer;

    QPixmap spriteSheet;
    float startY;
    float bobTime;

    int healAmount;
};

#endif // HEART_HPP