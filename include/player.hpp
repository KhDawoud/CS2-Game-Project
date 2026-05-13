#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include "maploader.hpp"
#include "lightningattack.hpp"

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
struct Fireball
{
    QGraphicsPixmapItem *item;
    QPointF direction;
};
struct Stats{
    int oldhealth;
    int oldstamina;
    int oldmana;
    int oldstaminaregenrate;
    int oldmanaregenrate;
    int olddamage;
    int health;
    int stamina;
    int mana;
    int staminaregenrate;
    int manaregenrate;
    int damage;
};

class Player : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Player(int);
    float getHealth() { return health; };
    void setHealth(float newHealth) { health = newHealth; emit statsChanged(); };
    float getStamina() { return stamina; };
    float getMana() { return mana; };
    void setMap(Map *m) { gameMap = m; }
    void setLevelsCompleted(int level){levelscleared = level;}
    void takeDamage(float damage);
    void Heal(float amount);
    QRectF getPlayerHitbox(QPointF pos) const;
    int getcharacternum();
    Stats getStats(){return playerstats;}
    int getLevelsCompleted(){return levelscleared;}
    void pickupkey(){key = true;}
    bool haskey(){return key;}

public slots:
    void updateAnimation();
    void movePlayer();


protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    float health;
    float stamina;
    float mana;
    Stats playerstats;

    float staminaRegenRate;
    float manaRegenRate;

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

    qreal sprintMultiplier = 1.5;
    bool isSprinting = false;
    int levelscleared=0;

signals:
    void positionChanged(QGraphicsItem *playerPtr); // The announcement
    void statsChanged();
    void playerDied();

private:
    QGraphicsRectItem *debugHitboxItem = nullptr;
    Map *gameMap = nullptr;
    QTimer *staminaRegenTimer;
    QTimer *manaRegenTimer;

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

    void regenMana(float amount)
    {
        mana = std::min(100.0f, mana + amount);
        emit statsChanged();
    };

    bool checkCollision(const QRectF &hitbox, MapLoader *map) const;
    void performAttack();
    void shootFireball();
    bool hasSpawnedFireball;
    bool isUsingLightning;
    bool isShiftPressed;

    bool isDashing = false;
    float dashSpeed = 15.0f;
    int dashDuration = 0;
    int dashCooldown = 0;
    QPointF dashDirection;
    LightningAttack* lightning;
    bool key = false;
};
#endif // PLAYER_HPP
