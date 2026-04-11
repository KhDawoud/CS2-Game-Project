#include "gameview.hpp"
#include <QGraphicsColorizeEffect>
#include <QKeyEvent>
#include <QProgressBar>
#include "map2.hpp"
#include "pausewindow.hpp"

GameView::GameView(Map *scene, House_Interior *interior, Player *player)
    : QGraphicsView(scene)
    , _overworld(scene)
    , _interior(interior)
    , _player(player)
{
    setAlignment(Qt::AlignCenter);

    scale(3.0, 3.0);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    centerOn(player);

    QProgressBar *progressBar = new QProgressBar(this);
    progressBar->setGeometry((scene->width() - 200) / 2, 20, 400, 25);
    progressBar->setStyleSheet("QProgressBar {"
                               "   background-color: rgba(50, 50, 50, 150);"
                               "   border: 3px solid #333;"
                               "   border-radius: 10px;"
                               "   color: white;"
                               "text-align: center;"
                               "}"
                               "QProgressBar::chunk {"
                               "   background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
                               "stop:0 #c0392b, stop:1 #e74c3c);"
                               "   border-radius: 7px;"
                               "}");
    progressBar->setRange(0, _overworld->getCurrentEnimies());
    progressBar->setValue(_overworld->getCurrentEnimies());
    connect(_overworld, &Map::requestBarUpdate, progressBar, &QProgressBar::setValue);
    connect(this, &GameView::isoverworld, progressBar, &QProgressBar::hide);

    showFullScreen();
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    // Escape Option
    if (event->key() == Qt::Key_Escape) {
        QGraphicsColorizeEffect *dim = new QGraphicsColorizeEffect();
        dim->setColor(Qt::black);
        dim->setStrength(0.7); // 70% dark
        this->setGraphicsEffect(dim);

        // Create the popup
        pausewindow window(this);

        // This stops the game (blocks) until the user clicks a button
        window.exec();
        this->setGraphicsEffect(nullptr);
    }
    // Switch Map Option
    else if (event->key() == Qt::Key_E) {
        // 1. Get current tile position (Flipped logic)

        int currentRow = _player->y() / 32;
        int currentCol = _player->x() / 32;

        // 2. Check if player is standing at the door in overworld
        if ((this->scene() == _overworld) && (currentRow >= 13 && currentRow <= 14)
            && (currentCol >= 8 && currentCol <= 10)) {
            // 3. Remove player from Overworld and move to Interior
            _overworld->removeItem(_player);
            this->setScene(_interior);
            emit isoverworld(false);
            _interior->addItem(_player);
            _player->setFocus();

            // 4. Set player spawn point inside the house
            _player->setPos(10.83 * 32, 7.25 * 32);

            this->centerOn(_player);
            this->setBackgroundBrush(Qt::black);
            this->resetTransform();
            this->scale(4.5, 4.5);
        }

        else if ((this->scene() == _interior) && (currentRow >= 7 && currentRow <= 7.5)
                 && (currentCol >= 10 && currentCol <= 11)) {
            _interior->removeItem(_player);
            this->setScene(_overworld);
            emit isoverworld(true);
            _overworld->addItem(_player);
            _player->setFocus();

            // 4. Set player spawn point inside the house
            _player->setPos(8.3 * 32, 14.5 * 32);

            this->centerOn(_player);
            this->resetTransform();
            this->scale(3.0, 3.0);
        }

    } else {
        // Pass other keys (movement) to the base class
        QGraphicsView::keyPressEvent(event);
    }
}
