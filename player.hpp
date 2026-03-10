#include <QGraphicsRectItem>
#include <QKeyEvent>

class Player : public QGraphicsRectItem{
public:
    Player();
protected:
    void keyPressEvent(QKeyEvent *event) override;
};