

#indef ENEMY_HPP
#define ENEMY_HPP

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QPixmap>

struct Position {
    float x, y;
    Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

struct Direction {
    float x, y;
    Direction(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

class BaseEnemy : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

protected:
    Position Pos;
    Direction Dir;
    int health;
    int maxHealth;
    float speed;
    int attack;
    int defense;
    int width, height;

    int currentFrame = 0;
    int currentRow = 0;
    QPixmap spriteSheet;

public:
    BaseEnemy(int hp, int atk, int def, float spd = 1.0f);

    bool isalive();
    bool isdead();

    void TakeDamage(int amount);

    void moveEnemy();
    virtual void updateAnimation();

public slots:
    void update();
};
#endif
