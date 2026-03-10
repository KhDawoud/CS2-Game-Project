#include "player.hpp"
#include <QBrush>

Player::Player()
{
    setRect(0, 0, 50, 50);
    setBrush(Qt::blue);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

void Player::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left)
    {
        moveBy(-10, 0);
    }
    else if (event->key() == Qt::Key_Right)
    {
        moveBy(10, 0);
    }
    else if (event->key() == Qt::Key_Up)
    {
        moveBy(0, -10);
    }
    else if (event->key() == Qt::Key_Down)
    {
        moveBy(0, 10);
    }
}
