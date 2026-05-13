#include "gameview.hpp"
#include <QApplication>
#include <QGraphicsColorizeEffect>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QTimer>
#include "pausewindow.hpp"
#include "levelcleared.hpp"
#include "levelintro.hpp"
#include "statsupgrade.hpp"
#include "characterselectscreen.hpp"
#include "deathwindow.hpp"

GameView::GameView(MapLoader *overworld, MapLoader *interior, MapLoader *level2, MapLoader *level3, Characters *player, CharacterStats *stats)
    : QGraphicsView(player->scene()
                        ? qobject_cast<MapLoader *>(player->scene())
                        : overworld),
      _overworld(overworld),
      _interior(interior),
      _player(player), Level2(level2),
      Level3(level3),
      _statsOverlay(stats)
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

    _player->setScale(1.2);
    _player->setFocus();
    centerOn(_player);

    _progressBar = new QProgressBar(this);
    _progressBar->setGeometry((_overworld->width() - 400) / 2, 20, 400, 25);
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

    bindProgress(_overworld);
    bindProgress(Level2);
    bindProgress(Level3);

    connect(Level3, &MapLoader::bossHealthChanged, this, [this](int health, int maxHealth)
            {
        _bossHealthBar->setMaximum(maxHealth);
        _bossHealthBar->setValue(health); });

    // visible in overwold
    connect(this, &GameView::isoverworld, _progressBar, &QProgressBar::setVisible);

    connect(_overworld, &MapLoader::levelCleared, this, [this]()
            {
        _player->resetInputState();
        LevelSelectWindow::markLevelCompleted(1);
        LevelSelectWindow::clearContinueState();

        auto* vWindow = new LevelCleared(this, _player->getLevelsCompleted());

        connect(vWindow, &QDialog::accepted, this, [this]() {
            auto* sWindow = new Statsupgrade(this, _player->getStats(), _player->getLevelsCompleted());
            connect(sWindow, &QDialog::accepted, this, &GameView::switchToInterior);
            sWindow->show();
        });
        vWindow->show(); });

    connect(Level2, &MapLoader::levelCleared, this, [this]()
            {
        _player->resetInputState();
        LevelSelectWindow::markLevelCompleted(2);
        LevelSelectWindow::clearContinueState();
        auto* vWindow = new LevelCleared(this, _player->getLevelsCompleted());

        connect(vWindow, &QDialog::accepted, this, [this]() {
            auto* sWindow = new Statsupgrade(this, _player->getStats(), _player->getLevelsCompleted());
            connect(sWindow, &QDialog::accepted, this, &GameView::switchToInterior);
            sWindow->show();
        });
        vWindow->show(); });

    connect(Level3, &MapLoader::levelCleared, this, [this]()
            {
        _player->resetInputState();
        LevelSelectWindow::markLevelCompleted(3);
        LevelSelectWindow::clearContinueState();
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
            _currentLightRadius = _baseLightRadius + ((slowWave + fastWave) * 5.0);
            viewport()->update(); 
        } });
    _flickerTimer->start(30);
    connect(player, &Player::playerDied, [this]()
            {
        DeathWindow deathScreen(this);
        if (deathScreen.exec() == QDialog::Accepted) {
            if (_player->getLevelsCompleted()+1 == 1) switchToOverworld();
            else if (_player->getLevelsCompleted()+1 == 2) switchtoLevel2();
            else if (_player->getLevelsCompleted()+1 == 3) switchtoLevel3();

            _player->setHealth(100 +_player->getLevelsCompleted()*20);
            _player->setMana(100 +_player->getLevelsCompleted()*20);
            _player->setStamina(100 +_player->getLevelsCompleted()*20);
            _player->setAnimationState((PlayerState::Idle));
        } });
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        _player->resetInputState();
        bool inLevel = (scene() == _overworld || scene() == Level2 || scene() == Level3);

        QGraphicsColorizeEffect *dim = new QGraphicsColorizeEffect(this);
        dim->setColor(Qt::black);
        dim->setStrength(0.7);
        this->setGraphicsEffect(dim);

        pausewindow window(this, inLevel);
        window.exec();
        this->setGraphicsEffect(nullptr);

        if (window.savedAndQuit)
            saveCurrentState();
    }
    else if (event->key() == Qt::Key_E)
    {
        int tileSize = static_cast<MapLoader *>(scene())->tileSize();
        float row = _player->y() / tileSize;
        float col = _player->x() / tileSize;

        if (scene() == _overworld && interactPrompt->isVisible() && row >= 13 && row <= 14 && col >= 8 && col <= 9)
        {
            _player->resetInputState();
            switchToInterior();
        }
        else if (scene() == _interior && interactPrompt->isVisible() && row >= 7 && row <= 8 && col >= 10 && col <= 12)
        {
            _player->resetInputState();
            openLevelSelect();
        }
    }
    else if (event->key() == Qt::Key_7)
    {
        switchToCharacterSelectScreen();
    }
    else if (event->key() == Qt::Key_8) // TEMP: jump to level 2
    {
        switchtoLevel2();
    }
    else if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_4)
    {
        int targetChar = event->key() - Qt::Key_1 + 1;
        if (_player->getcharacternum() == targetChar)
            return;
        _player->swtichto(targetChar);
    }
    else if (event->key() == Qt::Key_9)
    {
        if (scene() != _interior)
        {
            MapLoader *Thismap = dynamic_cast<MapLoader *>(scene());
            Thismap->levelCleared();
        }
    }
    else if (event->key() == Qt::Key_0)
    {
        if (scene() == Level2)
        {
            _baseLightRadius += 10.0; // Increase the radius by 10 each press
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
    bool wasinOverworld = false;
    scene()->removeItem(_player);
    if (scene() == _overworld || scene() == Level2)
    {
        scene()->removeItem(interactPrompt);
        scene()->removeItem(textStart);
        scene()->removeItem(textEnd);
        wasinOverworld = true;
    }

    setScene(_interior);
    emit isoverworld(false);
    currentscale = 1.2;

    _interior->addItem(_player);
    _player->setFocus();
    _player->setPos(10.83 * tileSize, 7.25 * tileSize);
    _player->setScale(currentscale);

    setBackgroundBrush(Qt::black);
    resetTransform();
    scale(4.5, 4.5);
    centerOn(_player);
    if (wasinOverworld)
    {
        _interior->addItem(interactPrompt);
        _interior->addItem(textStart);
        _interior->addItem(textEnd);
    }
}

void GameView::switchToOverworld()
{
    int tileSize = _overworld->tileSize();
    if (scene() == _overworld)
    {
        _player->setPos(8.3 * tileSize, 14.5 * tileSize);
        centerOn(_player);
        return;
    }
    _interior->removeItem(_player);
    _interior->removeItem(interactPrompt);
    _interior->removeItem(textStart);
    _interior->removeItem(textEnd);

    setScene(_overworld);
    emit isoverworld(true);
    currentscale = 1.2;

    _overworld->addItem(_player);
    _player->setFocus();
    _player->setPos(8.3 * tileSize, 14.5 * tileSize);
    _player->setScale(currentscale);

    setBackgroundBrush(Qt::NoBrush);
    resetTransform();
    scale(3.0, 3.0);
    centerOn(_player);
    _overworld->addItem(interactPrompt);
    scene()->addItem(textStart);
    scene()->addItem(textEnd);
    bindProgress(_overworld);
}

void GameView::switchToCharacterSelectScreen()
{
    CharacterSelectScreen *screen = new CharacterSelectScreen(_player->getcharacternum(), this);
    _player->resetInputState();

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

void GameView::switchtoLevel2()
{
    if (!Level2 || !_player)
    {
        return;
    }
    int tileSize = Level2->tileSize();

    if (scene() == Level2)
    {
        _player->setPos(10 * tileSize, 3 * tileSize);
        centerOn(_player);
        return;
    }

    _interior->removeItem(_player);
    _interior->removeItem(interactPrompt);
    _interior->removeItem(textStart);
    _interior->removeItem(textEnd);

    setScene(Level2);
    emit isoverworld(true);
    currentscale = 1.4;

    Level2->addItem(_player);
    _player->setFocus();
    _player->setPos(10 * tileSize, 3 * tileSize);
    _player->setScale(currentscale);

    setBackgroundBrush(Qt::NoBrush);
    resetTransform();
    scale(3.5, 3.5);
    centerOn(_player);
    scene()->addItem(interactPrompt);
    scene()->addItem(textStart);
    scene()->addItem(textEnd);
    bindProgress(Level2);
}

void GameView::switchtoLevel3()
{
    if (!Level3 || !_player)
    {
        return;
    }
    int tileSize = Level3->tileSize();

    if (scene() == Level3)
    {
        _player->setPos(18.5 * tileSize, 31 * tileSize);
        centerOn(_player);
        return;
    }
    _interior->removeItem(_player);

    setScene(Level3);
    emit isoverworld(true);
    currentscale = 1.2;

    Level3->addItem(_player);
    _player->setFocus();
    _player->setPos(18.5 * tileSize, 31 * tileSize);
    _player->setScale(currentscale);

    setBackgroundBrush(Qt::NoBrush);
    resetTransform();
    scale(3.5, 3.5);
    centerOn(_player);
    _bossHealthBar->setMaximum(300);
    _bossHealthBar->setValue(300);
    _bossHealthBar->setVisible(true);
    bindProgress(Level3);
}
void GameView::bindProgress(MapLoader *map)
{
    _progressBar->setRange(0, map->getInitialEnemyCount());
    _progressBar->setValue(map->getCurrentEnemyCount());

    connect(map, &MapLoader::requestBarUpdate,
            _progressBar, &QProgressBar::setValue,
            Qt::UniqueConnection);
}

void GameView::saveCurrentState()
{
    int lvl = _player->getLevelsCompleted() + 1;
    if (lvl == 0)
        return;

    MapLoader *currentMap = static_cast<MapLoader *>(scene());
    LevelSelectWindow::saveContinueState(
        lvl,
        static_cast<int>(_player->getHealth()),
        static_cast<int>(_player->getMana()),
        static_cast<int>(_player->getStamina()),
        static_cast<float>(_player->x()),
        static_cast<float>(_player->y()),
        currentMap->getCurrentEnemyCount(),
        _player->getcharacternum());

    qApp->quit();
}

void GameView::restoreContinueState()
{
    if (!LevelSelectWindow::hasContinueState())
        return;

    int lvl = LevelSelectWindow::getContinueLevel();
    int health = LevelSelectWindow::getContinueHealth();
    int mana = LevelSelectWindow::getContinueMana();
    int stamina = LevelSelectWindow::getContinueStamina();
    float posX = LevelSelectWindow::getContinuePosX();
    float posY = LevelSelectWindow::getContinuePosY();
    int charNum = LevelSelectWindow::getContinueCharacterNum();

    MapLoader *targetMap = nullptr;
    if (lvl == 1)
    {
        switchToOverworld();
        targetMap = _overworld;
    }
    else if (lvl == 2)
    {
        switchtoLevel2();
        targetMap = Level2;
    }
    else if (lvl == 3)
    {
        switchtoLevel3();
        targetMap = Level3;
    }

    _player->setLevelsCompleted(lvl - 1);
    _player->swtichto(charNum);
    _player->applyLevelProgress(lvl - 1, false);
    _player->setHealth(static_cast<float>(health));
    _player->setMana(static_cast<float>(mana));
    _player->setStamina(static_cast<float>(stamina));
    _player->setPos(posX, posY);
    _player->setScale(currentscale);

    // restore enemy count so bar and levelCleared fire from the right number
    if (targetMap)
    {
        int remaining = LevelSelectWindow::getContinueEnemiesKilled();
        targetMap->setCurrentEnemyCount(remaining);
        bindProgress(targetMap);
    }
}

void GameView::openLevelSelect()
{
    LevelSelectWindow *lsw = new LevelSelectWindow(this);
    lsw->setAttribute(Qt::WA_DeleteOnClose);
    _player->resetInputState();

    connect(lsw, &LevelSelectWindow::levelSelected, this, [this, lsw](int level)
            {
        lsw->close();
        if (level == 1)
            switchToOverworld();
        else if (level == 2)
            switchtoLevel2();
        else if (level == 3)
            switchtoLevel3(); });

    connect(lsw, &LevelSelectWindow::closeRequested, lsw, &LevelSelectWindow::close);
    connect(lsw, &LevelSelectWindow::continueRequested, this, [this, lsw]()
            {
        lsw->close();
        restoreContinueState(); });
    connect(lsw, &LevelSelectWindow::resetRequested, this, [this, lsw]()
            {
        lsw->close();
        openLevelSelect(); });

    // restore focus when the window closes without selecting a level
    connect(lsw, &QObject::destroyed, this, [this]()
            {
        this->activateWindow();
        this->setFocus();
        if (this->scene())
            this->scene()->setFocusItem(_player);
        _player->setFocus(); });

    lsw->show();
}

void GameView::checkInteractions()
{
    int tileSize = static_cast<MapLoader *>(scene())->tileSize();

    int row = static_cast<int>(_player->y() / tileSize);
    int col = static_cast<int>(_player->x() / tileSize);

    bool inZone = false;
    int doorId = 0;

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
    else if (scene() == Level2)
    {
        if (row >= 17 && row <= 19 && col >= 14 && col <= 16)
        {
            inZone = true;
            doorId = 1;
        }
        if (row >= 26 && row <= 30 && col >= 43 && col <= 46)
        {
            inZone = true;
            doorId = 2;
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
            textEnd->setPlainText("to Select Level");
        }
        else if (scene() == Level2 && doorId > 0)
        {
            if (_player->haskey(doorId))
            {
                textEnd->setPlainText("Door Unlocked");
            }
            else
            {
                textEnd->setPlainText("Key is needed to unlock the door");
            }
        }

        float w1 = textStart->boundingRect().width();
        float wIcon = interactPrompt->pixmap().width();
        float w2 = textEnd->boundingRect().width();
        float totalWidth = w1 + wIcon + w2;

        float startX = _player->x() + (_player->boundingRect().width() / 2) - (totalWidth / 2);

        float baseY;
        if (scene() == Level2)
        {
            baseY = _player->y() - textStart->boundingRect().height() - 10;
        }
        else
        {
            baseY = _player->y() + _player->boundingRect().height() + 10;
        }

        textStart->setPos(startX, baseY);

        float yoffset = 4.0f;
        float iconY = baseY + (textStart->boundingRect().height() / 2) -
                      (interactPrompt->pixmap().height() / 2) + yoffset;

        interactPrompt->setPos(startX + w1, iconY);
        textEnd->setPos(startX + w1 + wIcon - 10, baseY);

        if (scene() == Level2)
        {
            textStart->setVisible(false);
            interactPrompt->setVisible(false);
        }
        else
        {
            textStart->setVisible(true);
            interactPrompt->setVisible(true);
        }
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
    textStart = new QGraphicsTextItem("Press");
    textStart->setDefaultTextColor(Qt::white);
    textStart->setFont(QFont(pixelFontFamily, 6));

    interactPrompt = new QGraphicsPixmapItem(QPixmap(":resources/ui-elements/Ebutton.PNG"));
    interactPrompt->setZValue(100);
    interactPrompt->setVisible(false);
    interactPrompt->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    interactPrompt->setScale(1.8);

    textEnd = new QGraphicsTextItem("to Select Level");
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

        if (_statsOverlay && !_statsOverlay->pixmap().isNull())
        {
            painter->save();
            painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
            painter->translate(mapToScene(10, 10));
            painter->scale(_statsOverlay->scale(), _statsOverlay->scale());
            painter->drawPixmap(0, 0, _statsOverlay->pixmap());
            painter->restore();
        }
    }
}
