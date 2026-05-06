#include "gameview.hpp"
#include <QGraphicsColorizeEffect>
#include <QKeyEvent>
#include <QProgressBar>
#include "map2.hpp"
#include "pausewindow.hpp"
#include "levelcleared.hpp"
#include "levelintro.hpp"
#include "Enemy.hpp"

GameView::GameView(Map *scene, House_Interior *interior, Characters *player)

    : QGraphicsView(interior), _overworld(scene), _interior(interior), _player(player)

{
    setAlignment(Qt::AlignCenter);
    scale(4.5, 4.5);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _player->setScale(1.2);
    centerOn(player);

    // a progress bar that decreases as the number of enemies decreases
    QProgressBar *progressBar = new QProgressBar(this);
    progressBar->setGeometry((scene->width() - 200) / 2, 20, 400, 25);

    progressBar->setStyleSheet(
        "QProgressBar {"
        "   background-color: rgba(50, 50, 50, 150);"
        "   border: 3px solid #333;"
        "   border-radius: 10px;"
        "   color: white;"
        "text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #c0392b, stop:1 #e74c3c);"
        "   border-radius: 7px;"
        "}");
    progressBar->setRange(0, _overworld->getCurrentEnimies());
    progressBar->setValue(_overworld->getCurrentEnimies());
    connect(_overworld, &Map::requestBarUpdate, progressBar, &QProgressBar::setValue);
    connect(this, &GameView::isoverworld, progressBar, &QProgressBar::setVisible);

    connect(_overworld, &::Map::levelCleared, [this]()
            {
        LevelCleared clearedwindow(this);
        clearedwindow.exec(); });

    // creates the intro pop up when the game starts
    QTimer::singleShot(0, this, [this]()
                       {
                           LevelIntro *intro = new LevelIntro(this);
                           intro->raise();
                           intro->show();
                           connect(intro, &LevelIntro::introFinished, this, [this]() {
                               this->centerOn(_player);
                           });
                       });

    emit isoverworld(false);
    showFullScreen();
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    // Escape Option
    if (event->key() == Qt::Key_Escape)
    {
        QGraphicsColorizeEffect *dim = new QGraphicsColorizeEffect();
        dim->setColor(Qt::black);
        dim->setStrength(0.7); // 70% dark
        this->setGraphicsEffect(dim);

        pausewindow window(this);

        // This pauses the game until the user clicks a button
        window.exec();
        this->setGraphicsEffect(nullptr);
    }
    // Switch Map Option
    else if (event->key() == Qt::Key_E)
    {

        int currentRow = _player->y() / 32;
        int currentCol = _player->x() / 32;

        if ((this->scene() == _overworld) && (currentRow >= 13 && currentRow <= 14) && (currentCol >= 8 && currentCol <= 8.5))
        {

            _overworld->removeItem(_player);
            this->setScene(_interior);
            emit isoverworld(false);
            _interior->addItem(_player);
            _player->setScale(1.2f);
            _player->setFocus();
            _player->setPos(10.83 * 32, 7.25 * 32);

            this->centerOn(_player);
            this->setBackgroundBrush(Qt::black);
            this->resetTransform();
            this->scale(4.5, 4.5);
        }

        else if ((this->scene() == _interior) && (currentRow >= 7 && currentRow <= 7.5) && (currentCol >= 10 && currentCol <= 11))
        {

            _interior->removeItem(_player);
            this->setScene(_overworld);
            emit isoverworld(true);
            _overworld->addItem(_player);
            _player->setScale(1.2f);
            _player->setFocus();

            _player->setPos(8.3 * 32, 14.5 * 32);

            this->centerOn(_player);
            this->resetTransform();
            this->scale(3.0, 3.0);
        }
    }else if (event->key() == Qt::Key_1)
    {
        if(_player->getcharacternum() == 1) return;
        _player->swtichto(1);
        _player->setScale(1.2f);

    }else if (event->key() == Qt::Key_2)
    {
        if(_player->getcharacternum() == 2) return;
        _player->swtichto(2);
        _player->setScale(1.2f);
    }else if (event->key() == Qt::Key_3)
    {
        if(_player->getcharacternum() == 3) return;
        _player->swtichto(3);
        _player->setScale(1.2f);
    }
    else
    {
        // Pass other keys (movement) to the base class
        QGraphicsView::keyPressEvent(event);
    }
}


