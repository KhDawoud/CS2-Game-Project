#ifndef TRAPS_HPP
#define TRAPS_HPP

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QRectF>
#include <QTimer>

class Player;

// ===========================================================================
// SpikeTrap  -> resources/traps/trap1.png
// ===========================================================================

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

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;

    bool isExtended() const;
    void applyDamageTo(Player *player);
};

// ===========================================================================
// SlidingSpikeTrap  -> resources/traps/trap2.png
// ===========================================================================

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

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;

    QRectF spikeLocalRect() const;
    void applyDamageTo(Player *player);
};

// ===========================================================================
// SawBladeTrap  -> resources/traps/trap3.png
// ===========================================================================

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

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;

    void applyDamageTo(Player *player);
};

// ===========================================================================
// FireTrap  -> resources/traps/fire_trap1.png  (single row, 9 frames)
//
// Animation: simple forward loop  0 → 1 → … → 8 → 0
// setOffset() anchors the purple base to pos() — the item never drifts.
//
// Damage tiers:
//   frame 0        → no damage   (base plate, no flame)
//   frames 1-3     → sparkDamage (flat 3 HP, just sparks)
//   frames 4-8     → 25 % of the player's current health (full fire)
// ===========================================================================

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

    static const int frameCount = 9;   // frames 0–8 (single-row sheet)

    int currentFrame;    // advances 0→1→…→6→0
    int displayedFrame;  // copy used by damage logic

    float sparkDamage;   // flat HP cost during spark-only frames
    bool  canDamage;

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;

    bool isSparkFrame() const;          // true for frames 1–3
    bool isFireFrame()  const;          // true for frames 4–8
    void applyDamageTo(Player *player);
};

// ===========================================================================
// FireTrap2  — same sheet as FireTrap1 (fire_trap1.png), mirrored horizontally.
// Flames shoot RIGHT instead of LEFT. No separate sprite needed, no drifting.
// Damage tiers identical to FireTrap1.
// ===========================================================================

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

    static const int frameCount = 9;   // same as FireTrap1

    int currentFrame;
    int displayedFrame;

    float sparkDamage;
    bool  canDamage;

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;

    bool isSparkFrame() const;
    bool isFireFrame()  const;
    void applyDamageTo(Player *player);
};

// ===========================================================================
// FireTrap3  -> resources/traps/fire_trap3.png  (single horizontal row, 9 frames)
//
// Sheet: 1009x71, 9 frames -> frameWidth = 1009/9 = 112px (integer division),
// frameHeight = 71.
//
// Damage tiers identical to FireTrap1/2:
//   frame 0        -> no damage
//   frames 1-3     -> sparkDamage (flat 3 HP)
//   frames 4-8     -> 25% of player's current health
// ===========================================================================

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
    bool  canDamage;

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;

    bool isSparkFrame() const;
    bool isFireFrame()  const;
    void applyDamageTo(Player *player);
};

// ===========================================================================
// FireTrap4  — same sheet as FireTrap3 (fire_trap3.png), mirrored vertically.
// Flames shoot DOWN instead of UP. No separate sprite needed, no drifting.
// Damage tiers identical to FireTrap3.
// ===========================================================================

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
    bool  canDamage;

    QTimer *animationTimer;
    QTimer *damageTimer;
    QTimer *cooldownTimer;

    bool isSparkFrame() const;
    bool isFireFrame()  const;
    void applyDamageTo(Player *player);
};

#endif // TRAPS_HPP
