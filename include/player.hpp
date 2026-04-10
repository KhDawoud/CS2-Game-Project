#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>


class Map;

enum class PlayerState
{
    Idle,
    Walking,
    Attacking,
    Damaged,
    Dead
};

enum class Direction
{
    Right = 2,
    Up = 3,
    Left = 1,
    Down = 0
};

class Player : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Player();
    float getHealth() { return health; };
    float getStamina() { return stamina; };
    float getMana() { return mana; };
    void setMap(Map *m) { gameMap = m; }

public slots:
    void updateAnimation();
    void movePlayer();

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

signals:
    void positionChanged(QGraphicsItem *playerPtr); // The announcement
    void statsChanged();
    void playerDied();

private:
    QGraphicsRectItem *debugHitboxItem = nullptr;
    Map *gameMap = nullptr;
    float staminaRegenRate;
    QTimer *staminaRegenTimer;
    float health;
    float stamina;
    float mana;

    QTimer *animTimer;
    QTimer *idleTimer;

    PlayerState currentState;
    Direction currentDirection;
    int currentFrame;

    QPixmap walkSheet;
    QPixmap idleSheet;
    QPixmap attackSheet;
    QPixmap damagedSheet;
    QPixmap deadSheet;

    int walkFrameWidth;
    int walkFrameHeight;

    int idleFrameWidth;
    int idleFrameHeight;

    int attackFrameWidth;
    int attackFrameHeight;

    int damagedFrameWidth;
    int damagedFrameHeight;

    int deadFrameWidth;
    int deadFrameHeight;

    int maxFrames;

    QList<Qt::Key> activeKeys;

    void setAnimationState(PlayerState newState);
    void regenStamina(float amount)
    {
        stamina = std::min(100.0f, stamina + amount);
        emit statsChanged();
    };
    QRectF getPlayerHitbox(QPointF pos) const;
    bool checkCollision(const QRectF &hitbox, Map *map) const;
};
#endif // PLAYER_HPP
