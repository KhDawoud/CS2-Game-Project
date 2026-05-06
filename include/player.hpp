#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>

class Map;
class House_Interior;

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
    Player(int);
    float getHealth() { return health; };
    float getStamina() { return stamina; };
    float getMana() { return mana; };
    void setMap(Map *m) { gameMap = m; }
    void takeDamage(float damage);
    void Heal(float amount);
    QRectF getPlayerHitbox(QPointF pos) const;
    int getcharacternum();

public slots:
    void updateAnimation();
    void movePlayer();

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    float health;
    float stamina;
    float mana;

    QPixmap walkSheet;
    QPixmap idleSheet;
    QPixmap attackSheet;
    QPixmap damagedSheet;
    QPixmap deadSheet;

    int walkFrameWidth;
    int walkFrameHeight;
    int walkFrameWidthCount;

    int idleFrameWidth;
    int idleFrameHeight;
    int idleFrameWidthCount;
    int idleFrameUpCount;

    int attackFrameWidth;
    int attackFrameHeight;
    int attackFrameWidthCount;

    int damagedFrameWidth;
    int damagedFrameHeight;
    int damagedFrameWidthCount;

    int deadFrameWidth;
    int deadFrameHeight;
    int deadFrameWidthCount;

    int maxFrames;
    int characternum;
    int currentFrame;
    int animationfactor;
    int rowMap[4];
    int damage;

signals:
    void positionChanged(QGraphicsItem *playerPtr); // The announcement
    void statsChanged();
    void playerDied();

private:
    QGraphicsRectItem *debugHitboxItem = nullptr;
    Map *gameMap = nullptr;
    float staminaRegenRate;
    QTimer *staminaRegenTimer;

    QTimer *animTimer;
    QTimer *idleTimer;

    PlayerState currentState;
    Direction currentDirection;

    void paint();

    QList<Qt::Key> activeKeys;

    void setAnimationState(PlayerState newState);
    void regenStamina(float amount)
    {
        stamina = std::min(100.0f, stamina + amount);
        emit statsChanged();
    };

    bool checkCollision(const QRectF &hitbox, Map *map, House_Interior *interior) const;

    // bool checkCollision(const QRectF &hitbox, Map *map) const;
    void performAttack();
};
#endif // PLAYER_HPP
