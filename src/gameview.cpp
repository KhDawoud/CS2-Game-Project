#include "gameview.hpp"
#include <QGraphicsColorizeEffect>
#include <QKeyEvent>
#include <QTimer>
#include "pausewindow.hpp"
#include "levelcleared.hpp"
#include "levelintro.hpp"

GameView::GameView(MapLoader *overworld, MapLoader *interior, Characters *player)
    : QGraphicsView(player->scene()
                        ? qobject_cast<MapLoader *>(player->scene())
                        : overworld),
      _overworld(overworld),
      _interior(interior),
      _player(player)
{
    setAlignment(Qt::AlignCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // scale depends on where you are
    if (scene() == _interior)
    {
        setBackgroundBrush(Qt::black);
        scale(4.5, 4.5);
    }
    else
    {
        setBackgroundBrush(Qt::NoBrush);
        scale(3.0, 3.0);
    }
    _player->setScale(1.2);
    _player->setFocus();
    centerOn(_player);

    _progressBar = new QProgressBar(this);
    _progressBar->setGeometry((_overworld->width() - 400) / 2, 20, 400, 25);
    _progressBar->setStyleSheet(
        "QProgressBar { background-color: rgba(50,50,50,150); border: 3px solid #333; border-radius: 10px; color: white; text-align: center; }"
        "QProgressBar::chunk { background-color: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #c0392b, stop:1 #e74c3c); border-radius: 7px; }");

    // this essential allows the progress bar to work for any map regardless of enemy count or type
    auto bindProgress = [this](MapLoader *scene)
    {
        _progressBar->setRange(0, scene->getCurrentEnemyCount());
        _progressBar->setValue(scene->getCurrentEnemyCount());

        // UniqueConnection means if u switch maps it doesnt count as an update
        connect(scene, &MapLoader::requestBarUpdate,
                _progressBar, &QProgressBar::setValue,
                Qt::UniqueConnection);
    };

    bindProgress(_overworld);

    // visible in overwold
    connect(this, &GameView::isoverworld, _progressBar, &QProgressBar::setVisible);

    connect(_overworld, &MapLoader::levelCleared, this, [this]() {
        auto* clearedwindow = new LevelCleared(this);
        clearedwindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(clearedwindow, &QDialog::accepted, this, [this]() {
            this->switchToInterior();
        });
        clearedwindow->open();
    });

    QTimer::singleShot(0, this, [this]()
                       {
        LevelIntro *intro = new LevelIntro(this);
        intro->raise();
        intro->show();
        connect(intro, &LevelIntro::introFinished, this, [this]() {
            this->centerOn(_player);
        }); });

    emit isoverworld(scene() == _overworld);
    showFullScreen();
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        QGraphicsColorizeEffect *dim = new QGraphicsColorizeEffect(this);
        dim->setColor(Qt::black);
        dim->setStrength(0.7);
        this->setGraphicsEffect(dim);

        pausewindow window(this);
        window.exec();
        this->setGraphicsEffect(nullptr);
    }
    else if (event->key() == Qt::Key_E)
    {
        int tileSize = static_cast<MapLoader *>(scene())->tileSize();
        float row = _player->y() / tileSize;
        float col = _player->x() / tileSize;

        if (scene() == _overworld && row >= 13 && row <= 14 && col >= 8 && col <= 9)
        {
            switchToInterior();
        }
        else if (scene() == _interior && row >= 7 && row <= 8 && col >= 10 && col <= 11)
        {
            switchToOverworld();
        }
    }
    else if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_4)
    {
        int targetChar = event->key() - Qt::Key_1 + 1;
        if(_player->getcharacternum() == targetChar) return;
        _player->swtichto(targetChar);
        _player->setScale(1.2f);
    }
    else
    {
        QGraphicsView::keyPressEvent(event);
    }
}

void GameView::switchToInterior()
{
    int tileSize = _interior->tileSize();
    _overworld->removeItem(_player);

    setScene(_interior);
    emit isoverworld(false);

    _interior->addItem(_player);
    _player->setFocus();
    _player->setPos(10.83 * tileSize, 7.25 * tileSize);

    setBackgroundBrush(Qt::black);
    resetTransform();
    scale(4.5, 4.5);
    centerOn(_player);
}

void GameView::switchToOverworld()
{
    int tileSize = _overworld->tileSize();
    _interior->removeItem(_player);

    setScene(_overworld);
    emit isoverworld(true);

    _overworld->addItem(_player);
    _player->setFocus();
    _player->setPos(8.3 * tileSize, 14.5 * tileSize);

    setBackgroundBrush(Qt::NoBrush);
    resetTransform();
    scale(3.0, 3.0);
    centerOn(_player);
}
