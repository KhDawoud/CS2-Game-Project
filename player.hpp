#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>

enum class PlayerState
{
    Idle,
    Walking
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

public slots:
    void updateAnimation();
    void movePlayer();

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

private:
    QTimer *animTimer;
    QTimer *idleTimer;

    PlayerState currentState;
    Direction currentDirection;
    int currentFrame;

    QPixmap walkSheet;
    QPixmap idleSheet;

    int walkFrameWidth;
    int walkFrameHeight;

    int idleFrameWidth;
    int idleFrameHeight;

    int maxFrames;

    QList<Qt::Key> activeKeys;

    void setAnimationState(PlayerState newState);};
