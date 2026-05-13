#ifndef LIGHTNINGATTACK_HPP
#define LIGHTNINGATTACK_HPP

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPainter>
#include "maploader.hpp"
enum struct Direction;

enum class BoltState { Starting, Cycling, Ending, Inactive };

class LightningAttack : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    LightningAttack(QGraphicsItem* parent, MapLoader* Map);
    void startAttack();
    void stopAttack();
    void checkCollisions();
    void setMap(MapLoader* map) { currentMap = map; }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QRectF boundingRect() const override;
    void updateDirection(Direction dir);

public slots:
    void updateAnimation();

private:
    BoltState state = BoltState::Inactive;
    int currentFrame = 0;
    int frameWidth, frameHeight;
    QPixmap spriteSheet;
    MapLoader* currentMap;
    float currentLength;
    void updateLength();
};

#endif
