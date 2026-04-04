#include "gameview.hpp"
#include <QKeyEvent>
#include "pausewindow.hpp"
GameView::GameView(Map* scene,Player* player): QGraphicsView(scene) {
    setAlignment(Qt::AlignCenter);

    scale(3.0, 3.0);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    centerOn(player);
    showFullScreen();
}

void GameView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        // Create the popup
        pausewindow window(this);

        // This stops the game (blocks) until the user clicks a button
        window.exec();
    } else {
        // Pass other keys (movement) to the base class
        QGraphicsView::keyPressEvent(event);
    }
}
