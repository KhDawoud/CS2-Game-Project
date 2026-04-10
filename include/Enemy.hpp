#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "player.hpp"
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPixmap>
#include <QTimer>


struct Position
{
    float x, y;
    Position(float x = 0.0f, float y = 0.0f): x(x), y(y){}
};

struct EnemyDirection{
    float x, y;
    EnemyDirection(float x = 0.0f, float y = 0.0f): x(x), y(y){}
};
struct AnimData
{
    QPixmap sheet;
    int frameCount;
    int speedMs;
    int frameWidth;
    int frameHeight;
};

enum class EnemyState { Idle, Walking, Attacking, Hurt, Dead };

class BaseEnemy : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

protected:
    Position Pos;
    EnemyDirection Dir;
    int health;
    int maxHealth;
    float speed;
    int attack;
    int defense;

    AnimData idleData;
    AnimData walkData;
    AnimData attackData;
    AnimData deadData;
    AnimData hurtData;

    Player* player;

    void setEnemyState(EnemyState newState);
    int currentFrame = 0;
    int currentRow = 0;


    EnemyState currentState = EnemyState::Idle;

    QPixmap idleSheet;
    QPixmap walkSheet;
    QPixmap attackSheet;
    QPixmap deadSheet;
    QPixmap hurtSheet;

    float attackRange;
    int waitCounter = 0;
    int attackTimer = 0;
    int attackDuration = 10; // ~1 second


public:
    BaseEnemy(int hp, int atk, int def, float spd = 1.0f,float range=40.0f);
    void setPlayer(Player*p);
    bool isalive();
    bool isdead();

    void TakeDamage(int amount);

    void moveEnemy();
    virtual void updateAnimation();
    void detectandmove(Player*);

public slots:
    void update();
};



#endif
