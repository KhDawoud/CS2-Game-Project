#ifndef KEY_HPP
#define KEY_HPP

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>

class Player;

class Key : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Key(float x, float y, Player *p, int keyId, QGraphicsItem *parent = nullptr);

private slots:
    void updateTick();
    void despawn();

private:
    Player *player;
    QTimer *updateTimer;

    QPixmap spriteSheet;
    QPixmap shadowSpriteSheet;
    QGraphicsPixmapItem *shadow;

    float startY;
    float bobTime;
    int keyId;
};

#endif // KEY_HPP
