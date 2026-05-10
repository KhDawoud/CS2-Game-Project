#include "gameview.hpp"
#include <QGraphicsColorizeEffect>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QTimer>
#include "pausewindow.hpp"
#include "levelcleared.hpp"
#include "levelintro.hpp"
#include "statsupgrade.hpp"

GameView::GameView(MapLoader *overworld, MapLoader *interior, MapLoader *level2, MapLoader *level3, Characters *player)
    : QGraphicsView(player->scene()
                        ? qobject_cast<MapLoader *>(player->scene())
                        : overworld),
      _overworld(overworld),
      _interior(interior),
      _player(player), Level2(level2),
      Level3(level3)
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
    else if (scene() == Level2)
    {
        scale(4, 4);
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

    connect(_overworld, &MapLoader::levelCleared, this, [this]()
            {
        auto* vWindow = new LevelCleared(this, _player->getLevelsCompleted());

        connect(vWindow, &QDialog::accepted, this, [this]() {
            auto* sWindow = new Statsupgrade(this, _player->getStats(), _player->getLevelsCompleted());
            connect(sWindow, &QDialog::accepted, this, &GameView::switchToInterior);
            sWindow->show();
        });
        vWindow->show(); });

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
    loadinteractionPrompt();

    // this makes the torch expand and flicker to feel more dynamic
    _flickerTimer = new QTimer(this);
    connect(_flickerTimer, &QTimer::timeout, this, [this]()
            {
        if (scene() == Level2) 
        {
            _flickerPhase += 0.08; 
            if (_flickerPhase > 1000.0) { _flickerPhase = 0.0; } 
            qreal slowWave = std::sin(_flickerPhase); 
            qreal fastWave = std::sin(_flickerPhase * 2.7) * 0.5;
            _currentLightRadius = 120 + ((slowWave + fastWave) * 5.0);
            viewport()->update(); 
        } });
    _flickerTimer->start(30);
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

        if (scene() == _overworld && interactPrompt->isVisible() && row >= 13 && row <= 14 && col >= 8 && col <= 9)
        {
            switchToInterior();
        }
        else if (scene() == _interior && interactPrompt->isVisible() && row >= 7 && row <= 8 && col >= 10 && col <= 11)
        {
            switchToOverworld();
        }
    }
    else if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_4)
    {
        int targetChar = event->key() - Qt::Key_1 + 1;
        if (_player->getcharacternum() == targetChar)
            return;
        _player->swtichto(targetChar);
        _player->setScale(1.2f);
    }
    else if (event->key() == Qt::Key_9)
    {
        if (scene() == _interior)
        {
            switchtoLevel3();
        }
        else if (scene() == Level3)
        {
            switchToInterior();
        }
    }
    else if (event->key() == Qt::Key_8)
    {
        if (scene() == _interior)
        {
            switchtoLevel2();
        }
        else if (scene() == Level2)
        {
            switchToInterior();
        }
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
    _overworld->removeItem(interactPrompt);
    scene()->removeItem(textStart);
    scene()->removeItem(textEnd);

    setScene(_interior);
    emit isoverworld(false);

    _interior->addItem(_player);
    _player->setFocus();
    _player->setPos(10.83 * tileSize, 7.25 * tileSize);

    setBackgroundBrush(Qt::black);
    resetTransform();
    scale(4.5, 4.5);
    centerOn(_player);
    _interior->addItem(interactPrompt);
    scene()->addItem(textStart);
    scene()->addItem(textEnd);
}

void GameView::switchToOverworld()
{
    int tileSize = _overworld->tileSize();
    _interior->removeItem(_player);
    _interior->removeItem(interactPrompt);
    scene()->removeItem(textStart);
    scene()->removeItem(textEnd);

    setScene(_overworld);
    emit isoverworld(true);

    _overworld->addItem(_player);
    _player->setFocus();
    _player->setPos(8.3 * tileSize, 14.5 * tileSize);

    setBackgroundBrush(Qt::NoBrush);
    resetTransform();
    scale(3.0, 3.0);
    centerOn(_player);
    _overworld->addItem(interactPrompt);
    scene()->addItem(textStart);
    scene()->addItem(textEnd);
}

void GameView::switchtoLevel2()
{
    if (!Level2 || !_player)
    {
        return;
    }
    int tileSize = Level2->tileSize();
    this->scene()->removeItem(_player);

    setScene(Level2);
    emit isoverworld(true);

    Level2->addItem(_player);
    _player->setFocus();
    _player->setPos(10 * tileSize, 3 * tileSize);
    _player->setScale(1.4f);

    setBackgroundBrush(Qt::NoBrush);
    resetTransform();
    scale(3.5, 3.5);
    centerOn(_player);
}

void GameView::switchtoLevel3()
{
    if (!Level3 || !_player)
    {
        return;
    }
    int tileSize = Level3->tileSize();
    this->scene()->removeItem(_player);

    setScene(Level3);
    emit isoverworld(true);

    Level3->addItem(_player);
    _player->setFocus();
    _player->setPos(18.5 * tileSize, 31 * tileSize);

    setBackgroundBrush(Qt::NoBrush);
    resetTransform();
    scale(3.0, 3.0);
    centerOn(_player);
}
void GameView::checkInteractions()
{
    int tileSize = static_cast<MapLoader *>(scene())->tileSize();

    int row = static_cast<int>(_player->y() / tileSize);
    int col = static_cast<int>(_player->x() / tileSize);

    bool inZone = false;

    if (scene() == _overworld)
    {
        if (row >= 13 && row <= 14 && col >= 8 && col <= 9)
        {
            inZone = true;
        }
    }
    else if (scene() == _interior)
    {
        if (row >= 7 && row <= 8 && col >= 10 && col <= 11)
        {
            inZone = true;
        }
    }

    if (inZone)
    {
        if (scene() == _overworld)
        {
            textEnd->setPlainText("to Enter");
        }
        else if (scene() == _interior)
        {
            textEnd->setPlainText("to Exit");
        }

        float w1 = textStart->boundingRect().width();
        float wIcon = interactPrompt->pixmap().width();
        float w2 = textEnd->boundingRect().width();

        float totalWidth = w1 + wIcon + w2;

        float startX = _player->x() + (_player->boundingRect().width() / 2) - (totalWidth / 2);
        float baseY = _player->y() + _player->boundingRect().height() + 10;

        textStart->setPos(startX, baseY);

        float yoffset = 4.0f;

        float iconY = baseY + (textStart->boundingRect().height() / 2) -
                      (interactPrompt->pixmap().height() / 2) + yoffset;

        interactPrompt->setPos(startX + w1, iconY);

        textEnd->setPos(startX + w1 + wIcon - 10, baseY);

        textStart->setVisible(true);
        interactPrompt->setVisible(true);
        textEnd->setVisible(true);
    }
    else
    {
        textStart->setVisible(false);
        interactPrompt->setVisible(false);
        textEnd->setVisible(false);
    }
}
void GameView::loadinteractionPrompt()
{
    int fontId = QFontDatabase::addApplicationFont(":/resources/fonts/pixelfont.ttf");

    if (fontId != -1)
    {
        pixelFontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    }
    else
    {
        qWarning() << "CRITICAL: Could not load pixelfont.ttf! Using fallback font.";
        pixelFontFamily = "Arial";
    }
    textStart = new QGraphicsTextItem("Press");
    textStart->setDefaultTextColor(Qt::white);
    textStart->setFont(QFont(pixelFontFamily, 6));

    interactPrompt = new QGraphicsPixmapItem(QPixmap(":resources/ui-elements/Ebutton.PNG"));
    interactPrompt->setZValue(100);
    interactPrompt->setVisible(false);
    interactPrompt->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    interactPrompt->setScale(1.8);

    textEnd = new QGraphicsTextItem("to Enter");
    textEnd->setDefaultTextColor(Qt::white);
    textEnd->setFont(QFont(pixelFontFamily, 6));

    textStart->setZValue(100);
    interactPrompt->setZValue(100);
    textEnd->setZValue(100);

    textStart->setVisible(false);
    interactPrompt->setVisible(false);
    textEnd->setVisible(false);

    scene()->addItem(interactPrompt);
    scene()->addItem(textStart);
    scene()->addItem(textEnd);
}

// this is the method to handle drawing effects
void GameView::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawForeground(painter, rect);

    if (scene() == Level2)
    {
        QPointF playerCenter = _player->sceneBoundingRect().center();
        QRadialGradient gradient(playerCenter, _currentLightRadius);

        // its slightly yellow here to be like fire but we can tweak it
        gradient.setColorAt(0.0, QColor(255, 215, 100, 35));
        gradient.setColorAt(0.4, QColor(80, 50, 20, 120));
        gradient.setColorAt(0.7, QColor(10, 5, 0, 220));
        gradient.setColorAt(1.0, QColor(0, 0, 0, 255));

        painter->fillRect(rect, gradient);
    }
}