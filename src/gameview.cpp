#include "gameview.hpp"
#include <QGraphicsColorizeEffect>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QRandomGenerator>
#include <QTimer>
#include "pausewindow.hpp"
#include "levelcleared.hpp"
#include "levelintro.hpp"
#include "statsupgrade.hpp"
#include "characterselectscreen.hpp"
#include "levelselectwindow.hpp"
#include <QApplication>

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
    // geometry is set properly in resizeEvent once the window has an actual pixel size
    _progressBar->setGeometry(0, 20, 400, 25);
    _progressBar->setStyleSheet(
        "QProgressBar { background-color: rgba(50,50,50,150); border: 3px solid #333; border-radius: 10px; color: white; text-align: center; }"
        "QProgressBar::chunk { background-color: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #c0392b, stop:1 #e74c3c); border-radius: 7px; }");

    _bossLabel = new QLabel("BOSS", this);
    _bossLabel->setStyleSheet("color: #ff3333; font-size: 24px; font-weight: bold; background: transparent; font-family: 'Courier New';");
    _bossLabel->setAlignment(Qt::AlignCenter);

    _bossHealthBar = new QProgressBar(this);
    _bossHealthBar->setFormat("%v / %m");
    _bossHealthBar->setAlignment(Qt::AlignCenter);
    _bossHealthBar->setStyleSheet(
        "QProgressBar { border: 3px solid #3b2121; background-color: #2b2b2b; border-radius: 5px; color: white; font-weight: bold; font-size: 20px; }"
        "QProgressBar::chunk { background-color: #d32f2f; border-radius: 2px; }");

    QRect progRect = _progressBar->geometry();
    _bossHealthBar->setGeometry(progRect.x(), progRect.y() + 30, progRect.width(), progRect.height() + 20);
    _bossLabel->setGeometry(progRect.x(), progRect.y(), progRect.width(), 30);
    _bossHealthBar->setVisible(false);
    _bossLabel->setVisible(false);

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

    // visible in overworld
    connect(this, &GameView::isoverworld, _progressBar, &QProgressBar::setVisible);

    auto showLevelClearedWindow = [this]() {
        auto* vWindow = new LevelCleared(this, _player->getLevelsCompleted());

        connect(vWindow, &QDialog::accepted, this, [this]() {
            // Single read-modify-write: mark completed AND clear continue in one pass.
            int level = currentLevelNumber();
            QJsonObject save = LevelSelectWindow::readSave();
            if (level > save.value("completedLevels").toInt(0))
                save["completedLevels"] = level;
            int nextUnlock = qMin(level + 1, 3);
            if (nextUnlock > save.value("unlockedLevel").toInt(1))
                save["unlockedLevel"] = nextUnlock;
            QJsonObject cont = save.value("continue").toObject();
            cont["exists"] = false;
            save["continue"] = cont;
            LevelSelectWindow::writeSave(save);

            auto* sWindow = new Statsupgrade(this, _player->getStats(), _player->getLevelsCompleted());
            connect(sWindow, &QDialog::accepted, this, &GameView::switchToInterior);
            sWindow->show();
        });
        vWindow->show();
    };

    connect(_overworld, &MapLoader::levelCleared, this, showLevelClearedWindow);
    connect(Level2,   &MapLoader::levelCleared, this, showLevelClearedWindow);
    connect(Level3,   &MapLoader::levelCleared, this, showLevelClearedWindow);

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

        bool inLevel = scene() != _interior;
        pausewindow window(this, inLevel);
        window.exec();
        this->setGraphicsEffect(nullptr);

        if (window.savedAndQuit)
        {
            int currentLevel = currentLevelNumber();
            int health = static_cast<int>(_player->getHealth());
            int enemiesKilled = _progressBar->maximum() - _progressBar->value();

            LevelSelectWindow::saveContinueState(currentLevel,
                                                 health,
                                                 _player->x(),
                                                 _player->y(),
                                                 enemiesKilled,
                                                 _player->getcharacternum());
            qApp->quit();
            return;
        }
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
    else if (event->key() == Qt::Key_7)
    {
        switchToCharacterSelectScreen();
    }
    else if (event->key() == Qt::Key_L)
    {
        openLevelSelect();
    }
    else if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_4)
    {
        int targetChar = event->key() - Qt::Key_1 + 1;
        if (_player->getcharacternum() == targetChar)
            return;
        _player->swtichto(targetChar);
        _player->setScale(currentscale);
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
    changeScene(_interior, 10.83 * tileSize, 7.25 * tileSize, 1.2, 4.5);
}

void GameView::switchToOverworld()
{
    int tileSize = _overworld->tileSize();
    changeScene(_overworld, 8.3 * tileSize, 14.5 * tileSize, 1.2, 3.0);
}

void GameView::switchtoLevel2()
{
    if (!Level2 || !_player)
    {
        return;
    }
    int tileSize = Level2->tileSize();
    changeScene(Level2, 10 * tileSize, 3 * tileSize, 1.4, 3.5);
    _progressBar->setRange(0, Level2->getTotalEnemyCount());
    _progressBar->setValue(Level2->getCurrentEnemyCount());
    connect(Level2, &MapLoader::requestBarUpdate,
            _progressBar, &QProgressBar::setValue,
            Qt::UniqueConnection);
}

void GameView::switchtoLevel3()
{
    if (!Level3 || !_player)
        return;
    int tileSize = Level3->tileSize();
    changeScene(Level3, 18.5 * tileSize, 31 * tileSize, 1.2, 3.5);
    _bossHealthBar->setMaximum(300);
    _bossHealthBar->setValue(300);
    _bossHealthBar->setVisible(true);
    _bossLabel->setVisible(true);
    _progressBar->setRange(0, Level3->getTotalEnemyCount());
    _progressBar->setValue(Level3->getCurrentEnemyCount());
    connect(Level3, &MapLoader::requestBarUpdate,
            _progressBar, &QProgressBar::setValue,
            Qt::UniqueConnection);
}

void GameView::changeScene(MapLoader *target, qreal posX, qreal posY, qreal playerScale, qreal viewScale)
{
    if (!target || !_player)
        return;

    MapLoader *currentScene = qobject_cast<MapLoader *>(scene());
    if (currentScene)
    {
        currentScene->removeItem(_player);
        if (currentScene == _overworld || currentScene == _interior)
        {
            currentScene->removeItem(interactPrompt);
            currentScene->removeItem(textStart);
            currentScene->removeItem(textEnd);
        }
    }

    setScene(target);
    emit isoverworld(target != _interior);
    currentscale = playerScale;

    target->addItem(_player);
    _player->setFocus();
    _player->setPos(posX, posY);
    _player->setScale(currentscale);

    setBackgroundBrush(target == _interior ? QBrush(Qt::black) : QBrush(Qt::NoBrush));
    resetTransform();
    scale(viewScale, viewScale);
    centerOn(_player);

    if (target == _interior || target == _overworld)
    {
        target->addItem(interactPrompt);
        target->addItem(textStart);
        target->addItem(textEnd);
    }
}

int GameView::currentLevelNumber() const
{
    if (scene() == Level3)
        return 3;
    if (scene() == Level2)
        return 2;
    if (scene() == _overworld)
        return 1;
    return 0;
}

MapLoader *GameView::sceneForLevel(int level) const
{
    switch (level)
    {
    case 1: return _overworld;
    case 2: return Level2;
    case 3: return Level3;
    default: return nullptr;
    }
}

void GameView::openLevelSelect()
{
    auto *window = new LevelSelectWindow(this);
    // continueMode starts false — doors show normal state until the player clicks Continue

    connect(window, &LevelSelectWindow::levelSelected, this, [this, window](int level) {
        window->close();
        // Starting a fresh level — discard any stale mid-game save so the
        // Continue button doesn't reappear with outdated state next time.
        LevelSelectWindow::clearContinueState();
        MapLoader *target = sceneForLevel(level);
        if (target)
        {
            if (level == 1)
                switchToOverworld();
            else if (level == 2)
                switchtoLevel2();
            else if (level == 3)
                switchtoLevel3();
        }
    });
    connect(window, &LevelSelectWindow::continueRequested, this, [this, window]() {
        window->close();

        // Read the save file once and extract everything from that one object.
        QJsonObject cont = LevelSelectWindow::readSave().value("continue").toObject();
        if (!cont.value("exists").toBool(false))
            return;

        int continueLevel  = cont.value("level").toInt(1);
        int characterNum   = cont.value("characterNum").toInt(1);
        float posX         = static_cast<float>(cont.value("posX").toDouble(0));
        float posY         = static_cast<float>(cont.value("posY").toDouble(0));
        int health         = cont.value("health").toInt(3);
        int enemiesKilled  = cont.value("enemiesKilled").toInt(0);

        if (characterNum >= 1 && characterNum <= 4)
            _player->swtichto(characterNum);

        MapLoader *target = sceneForLevel(continueLevel);
        if (!target)
            target = _overworld;

        if (target == _overworld)
            changeScene(_overworld, posX, posY, 1.2, 3.0);
        else if (target == Level2)
            changeScene(Level2, posX, posY, 1.4, 3.5);
        else if (target == Level3)
            changeScene(Level3, posX, posY, 1.2, 3.5);

        if (health > 0)
            _player->setHealth(static_cast<float>(health));

        int maxValue = target->getTotalEnemyCount();
        int remaining = qBound(0, maxValue - enemiesKilled, maxValue);
        target->setCurrentEnemyCount(remaining);
        _progressBar->setRange(0, maxValue);
        _progressBar->setValue(remaining);
        // Reconnect progress bar so it tracks kills on this scene going forward.
        connect(target, &MapLoader::requestBarUpdate,
                _progressBar, &QProgressBar::setValue,
                Qt::UniqueConnection);
    });
    connect(window, &LevelSelectWindow::closeRequested, window, &QWidget::close);

    window->show();
}

void GameView::switchToCharacterSelectScreen()
{
    CharacterSelectScreen *screen = new CharacterSelectScreen(_player->getcharacternum(), this);

    connect(screen, &CharacterSelectScreen::selectionMade, this, [this](int charIndex)
            {
        _player->swtichto(charIndex);
        _player->setScale(1.2f); });

    connect(screen, &QObject::destroyed, this, [this]()
            {
        this->activateWindow();
        this->setFocus();
        if (this->scene()) {
            this->scene()->setFocusItem(_player);
        }
        _player->setFocus(); });

    screen->raise();
    screen->show();
    screen->setFocus();
}

bool GameView::restoreContinueState()
{
    // Single file read — extract everything from one QJsonObject.
    QJsonObject cont = LevelSelectWindow::readSave().value("continue").toObject();
    if (!cont.value("exists").toBool(false))
        return false;

    int continueLevel  = cont.value("level").toInt(1);
    int characterNum   = cont.value("characterNum").toInt(1);
    float posX         = static_cast<float>(cont.value("posX").toDouble(0));
    float posY         = static_cast<float>(cont.value("posY").toDouble(0));
    int health         = cont.value("health").toInt(3);
    int enemiesKilled  = cont.value("enemiesKilled").toInt(0);

    if (characterNum >= 1 && characterNum <= 4)
        _player->swtichto(characterNum);

    MapLoader *target = sceneForLevel(continueLevel);
    if (!target)
        target = _overworld;

    if (target == _overworld)
        changeScene(_overworld, posX, posY, 1.2, 3.0);
    else if (target == Level2)
        changeScene(Level2, posX, posY, 1.4, 3.5);
    else if (target == Level3)
        changeScene(Level3, posX, posY, 1.2, 3.5);

    if (health > 0)
        _player->setHealth(static_cast<float>(health));

    int maxValue = target->getTotalEnemyCount();
    int remaining = qBound(0, maxValue - enemiesKilled, maxValue);
    target->setCurrentEnemyCount(remaining);
    _progressBar->setRange(0, maxValue);
    _progressBar->setValue(remaining);
    connect(target, &MapLoader::requestBarUpdate,
            _progressBar, &QProgressBar::setValue,
            Qt::UniqueConnection);

    return true;
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
            textStart->setPlainText("Press");
            interactPrompt->setVisible(true);
            textEnd->setPlainText("to Enter");
        }
    }
    else if (scene() == _interior)
    {
        if (row >= 7 && row <= 8 && col >= 10 && col <= 11)
        {
            inZone = true;
            textStart->setPlainText("Press");
            interactPrompt->setVisible(true);
            textEnd->setPlainText("to Exit");
        }
    }

    if (inZone)
    {
        float w1 = textStart->boundingRect().width();
        float wIcon = interactPrompt->isVisible() ? interactPrompt->pixmap().width() : 0;
        float w2 = textEnd->boundingRect().width();

        float totalWidth = w1 + wIcon + w2;

        float startX = _player->x() + (_player->boundingRect().width() / 2) - (totalWidth / 2);
        float baseY = _player->y() + _player->boundingRect().height() + 10;

        textStart->setPos(startX, baseY);

        float yoffset = 4.0f;

        if (interactPrompt->isVisible())
        {
            float iconY = baseY + (textStart->boundingRect().height() / 2) -
                          (interactPrompt->pixmap().height() / 2) + yoffset;
            interactPrompt->setPos(startX + w1, iconY);
        }

        textEnd->setPos(startX + w1 + wIcon - 10, baseY);

        textStart->setVisible(true);
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