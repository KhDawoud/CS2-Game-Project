#ifndef TRAPS_HPP
#define TRAPS_HPP

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QRectF>
#include <QTimer>

class Player;

class SpikeTrap : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit SpikeTrap(QGraphicsItem *parent = nullptr);

    int getwidth() const;
    int getheight() const;

    void setDamage(float amount);
    void setDamageCooldown(int milliseconds);

    QRectF damageArea() const;

public slots:
    void UpdateAnimation();
    void CheckDamage();

private:
    QPixmap framesheet;

    int frameWidth;
    int frameHeight;

    static const int frameCount = 6;

    int currentFrame;
    int displayedFrame;

    float damage;
    bool canDamage;
    bool isPaused;

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;
    QTimer *pauseTimer;

    bool isExtended() const;
    void applyDamageTo(Player *player);
};

class SlidingSpikeTrap : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit SlidingSpikeTrap(QGraphicsItem *parent = nullptr);

    int getwidth() const;
    int getheight() const;

    void setDamage(float amount);
    void setDamageCooldown(int milliseconds);

    QRectF damageArea() const;

public slots:
    void UpdateAnimation();
    void CheckDamage();

private:
    QPixmap framesheet;

    int frameWidth;
    int frameHeight;

    static const int frameCount = 6;

    int currentFrame;
    int displayedFrame;
    int direction;

    float damage;
    bool canDamage;
    bool isPaused;

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;
    QTimer *pauseTimer;

    QRectF spikeLocalRect() const;
    void applyDamageTo(Player *player);
};

class SawBladeTrap : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit SawBladeTrap(QGraphicsItem *parent = nullptr);

    int getwidth() const;
    int getheight() const;

    void setDamage(float amount);
    void setDamageCooldown(int milliseconds);

    QRectF damageArea() const;

public slots:
    void UpdateAnimation();
    void CheckDamage();

private:
    QPixmap framesheet;

    int frameWidth;
    int frameHeight;

    static const int frameCount = 6;

    int currentFrame;
    int displayedFrame;

    float damage;
    bool canDamage;
    bool isPaused;

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;
    QTimer *pauseTimer;

    void applyDamageTo(Player *player);
};

class FireTrap : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit FireTrap(QGraphicsItem *parent = nullptr);

    int getwidth() const;
    int getheight() const;

    QRectF damageArea() const;

public slots:
    void UpdateAnimation();
    void CheckDamage();

private:
    QPixmap framesheet;

    int frameWidth;
    int frameHeight;

    static const int frameCount = 9;

    int currentFrame;
    int displayedFrame;

    float sparkDamage;
    bool canDamage;
    bool isPaused;

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;
    QTimer *pauseTimer;

    bool isSparkFrame() const;
    bool isFireFrame() const;
    void applyDamageTo(Player *player);
};

class FireTrap2 : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit FireTrap2(QGraphicsItem *parent = nullptr);

    int getwidth() const;
    int getheight() const;

    QRectF damageArea() const;

public slots:
    void UpdateAnimation();
    void CheckDamage();

private:
    QPixmap framesheet;

    int frameWidth;
    int frameHeight;

    static const int frameCount = 9;

    int currentFrame;
    int displayedFrame;

    float sparkDamage;
    bool canDamage;
    bool isPaused;

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;
    QTimer *pauseTimer;

    bool isSparkFrame() const;
    bool isFireFrame() const;
    void applyDamageTo(Player *player);
};

class FireTrap3 : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit FireTrap3(QGraphicsItem *parent = nullptr);

    int getwidth() const;
    int getheight() const;

    QRectF damageArea() const;

public slots:
    void UpdateAnimation();
    void CheckDamage();

private:
    QPixmap framesheet;

    int frameWidth;
    int frameHeight;

    static const int frameCount = 9;

    int currentFrame;
    int displayedFrame;

    float sparkDamage;
    bool canDamage;
    bool isPaused;

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;
    QTimer *pauseTimer;

    bool isSparkFrame() const;
    bool isFireFrame() const;
    void applyDamageTo(Player *player);
};

class FireTrap4 : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit FireTrap4(QGraphicsItem *parent = nullptr);

    int getwidth() const;
    int getheight() const;

    QRectF damageArea() const;

public slots:
    void UpdateAnimation();
    void CheckDamage();

private:
    QPixmap framesheet;

    int frameWidth;
    int frameHeight;

    static const int frameCount = 9;

    int currentFrame;
    int displayedFrame;

    float sparkDamage;
    bool canDamage;
    bool isPaused;

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;
    QTimer *pauseTimer;

    bool isSparkFrame() const;
    bool isFireFrame() const;
    void applyDamageTo(Player *player);
};

#endif