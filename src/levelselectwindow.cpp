#include "levelselectwindow.hpp"
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QDir>


QString LevelSelectWindow::savePath()
{
    // here we just choose where the save file will live on the user's pc
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/savegame.json";
}

QJsonObject LevelSelectWindow::readSave()
{
    // here we read the save file and return an empty object if there is no save yet
    QFile f(savePath());

    if (!f.open(QIODevice::ReadOnly))
        return QJsonObject();

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    return doc.isObject() ? doc.object() : QJsonObject();
}

void LevelSelectWindow::writeSave(const QJsonObject &obj)
{
    // here we rewrite the save file with the newest progress
    QFile f(savePath());

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    f.close();
}

static constexpr int TOTAL_LEVELS = 3;
static int sessionCompletedLevels = 0;
static int sessionUnlockedLevel = 1;

void LevelSelectWindow::markLevelCompleted(int level)
{
    // here we mark the finished level and unlock only the next door
    if (level > sessionCompletedLevels)
        sessionCompletedLevels = level;

    sessionUnlockedLevel = qMax(sessionUnlockedLevel, qMin(level + 1, TOTAL_LEVELS));
}

void LevelSelectWindow::saveContinueState(int level, int health,
                                          int mana, int stamina,
                                          float posX, float posY,
                                          int enemiesKilled,
                                          int characterNum)
{
    QJsonObject save = readSave();

    // here we save everything needed to continue from the same spot later
    QJsonObject cont;
    cont["exists"] = true;
    cont["level"] = level;
    cont["health"] = health;
    cont["mana"] = mana;
    cont["stamina"] = stamina;
    cont["posX"] = static_cast<double>(posX);
    cont["posY"] = static_cast<double>(posY);
    cont["enemiesKilled"] = enemiesKilled;
    cont["characterNum"] = characterNum;

    save["continue"] = cont;
    writeSave(save);
}

void LevelSelectWindow::clearContinueState()
{
    QJsonObject save = readSave();

    // here we keep the old data but tell the menu that continue is no longer active
    QJsonObject cont = save.value("continue").toObject();
    cont["exists"] = false;

    save["continue"] = cont;

    writeSave(save);
}

bool LevelSelectWindow::hasContinueState()
{
    return readSave().value("continue").toObject().value("exists").toBool(false);
}

int LevelSelectWindow::getContinueLevel()
{
    return readSave().value("continue").toObject().value("level").toInt(1);
}

int LevelSelectWindow::getContinueHealth()
{
    return readSave().value("continue").toObject().value("health").toInt(100);
}

int LevelSelectWindow::getContinueMana()
{
    QJsonObject cont = readSave().value("continue").toObject();
    int level = cont.value("level").toInt(1);
    return cont.value("mana").toInt(100 + ((level - 1) * 20));
}

int LevelSelectWindow::getContinueStamina()
{
    QJsonObject cont = readSave().value("continue").toObject();
    int level = cont.value("level").toInt(1);
    return cont.value("stamina").toInt(100 + ((level - 1) * 20));
}

float LevelSelectWindow::getContinuePosX()
{
    return static_cast<float>(
        readSave().value("continue").toObject().value("posX").toDouble(0)
        );
}

float LevelSelectWindow::getContinuePosY()
{
    return static_cast<float>(
        readSave().value("continue").toObject().value("posY").toDouble(0)
        );
}

int LevelSelectWindow::getContinueEnemiesKilled()
{
    return readSave().value("continue").toObject().value("enemiesKilled").toInt(0);
}

int LevelSelectWindow::getContinueCharacterNum()
{
    return readSave().value("continue").toObject().value("characterNum").toInt(1);
}


LevelSelectWindow::LevelSelectWindow(QWidget *parent)
    : QWidget(parent)
{
    continueMode = false;


    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/pixelfont.ttf");

    if (fontId != -1)
        pixelFontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(PANEL_SRC_W * SCALE, PANEL_SRC_H * SCALE);

    sheet.load(":resources/ui-elements/Levels.png");

    // here we cut the three door states from the same UI sprite sheet
    doorAvailable = sheet.copy(88, 162, 17, 22);
    doorLocked = sheet.copy(120, 162, 17, 22);
    doorCompleted = sheet.copy(56, 162, 17, 22);

    loadSaveData();
    setupButtons();
}

void LevelSelectWindow::loadSaveData()
{
    // here we decide what the menu should show: normal session progress or continue progress
    QJsonObject save = readSave();
    QJsonObject cont = save.value("continue").toObject();

    hasContinue = cont.value("exists").toBool(false);
    continueLevel = cont.value("level").toInt(1);
    if (continueMode && hasContinue)
    {
        // here we show the saved run progress instead of the current session progress
        completedLevels = qMax(0, continueLevel - 1);
        unlockedLevel = continueLevel;
    }
    else
    {
        completedLevels = sessionCompletedLevels;
        unlockedLevel = sessionUnlockedLevel;
    }

    if (unlockedLevel < 1)
        unlockedLevel = 1;

    if (completedLevels < 0)
        completedLevels = 0;

    if (continueLevel < 0)
        continueLevel = 0;
}

void LevelSelectWindow::setupButtons()
{
    for (int i = 0; i < 3; i++) {
        int level = i + 1;

        int cx = CIRCLE_CX[i] * SCALE;
        int cy = CIRCLE_CY * SCALE;
        int r = CIRCLE_R * SCALE;

        QPushButton *btn = new QPushButton(this);

        // here we add invisible buttons on top of the painted doors so they can be clicked
        btn->setGeometry(cx - r, cy - r, r * 2, r * 2);
        btn->setFlat(true);
        btn->setStyleSheet("background: transparent; border: none;");

        levelButtons.push_back(btn);

        connect(btn, &QPushButton::clicked, this, [this, level]() {
            emit levelSelected(level);
        });
    }

    if (hasContinue) {
        // here we only show continue/reset if a continue save actually exists
        QPushButton *continueBtn = new QPushButton("CONTINUE", this);
        QPushButton *reset = new QPushButton("RESET PROGRESS", this);

        if (!pixelFontFamily.isEmpty())
            continueBtn->setFont(QFont(pixelFontFamily, 9, QFont::Bold));
            reset->setFont(QFont(pixelFontFamily, 9, QFont::Bold));

        int cx = PANEL_SRC_W * SCALE / 2;
        int cy = (CIRCLE_CY + CIRCLE_R + 22) * SCALE;

        continueBtn->setGeometry(cx - 72, cy, 144, 28);

        continueBtn->setStyleSheet(
            "background-color: #4CAF50; color: black; "
            "border: 2px solid #2e7d32; border-radius: 4px;"
            );

        continueBtn->setCursor(Qt::PointingHandCursor);

        connect(continueBtn, &QPushButton::clicked, this, [this]() {
            // here we switch the menu into continue mode before starting the saved run
            setContinueMode(true);
            emit continueRequested();
        });
        cy += + 38;

        reset->setGeometry(cx - 72, cy, 144, 28);

        reset->setStyleSheet(
            "background-color: #4CAF50; color: black; "
            "border: 2px solid #2e7d32; border-radius: 4px;"
            );

        reset->setCursor(Qt::PointingHandCursor);

        connect(reset, &QPushButton::clicked, this, [this]() {
            // here we clear the saved run and tell the main window to reset progress
            clearContinueState();
            emit resetRequested();
        });
    }

    QPushButton *quitBtn = new QPushButton("QUIT", this);

    // here we make the quit button separate from the door buttons
    if (!pixelFontFamily.isEmpty())
        quitBtn->setFont(QFont(pixelFontFamily, 8, QFont::Bold));

    quitBtn->setGeometry(QUIT_X * SCALE,
                         QUIT_Y * SCALE,
                         QUIT_W * SCALE,
                         QUIT_H * SCALE);

    quitBtn->setStyleSheet(
        "background-color: rgba(139,90,60,200); color: #f5e0c0; "
        "border: 2px solid #5c3a1e; border-radius: 3px;"
        );

    quitBtn->setCursor(Qt::PointingHandCursor);

    connect(quitBtn, &QPushButton::clicked,
            this, &LevelSelectWindow::closeRequested);

    QPushButton *closeBtn = new QPushButton(this);

    // here we use a hidden button for the X/close area drawn on the panel art
    closeBtn->setGeometry(CLOSE_X * SCALE,
                          CLOSE_Y * SCALE,
                          CLOSE_W * SCALE,
                          CLOSE_H * SCALE);

    closeBtn->setFlat(true);
    closeBtn->setStyleSheet("background: transparent; border: none;");
    closeBtn->setCursor(Qt::PointingHandCursor);

    connect(closeBtn, &QPushButton::clicked,
            this, &LevelSelectWindow::closeRequested);

    updateButtonStates();
}

void LevelSelectWindow::forceUnlockAll()
{
    // here we unlock everything for testing or debug use
    sessionUnlockedLevel = 3;
    sessionCompletedLevels = 3;
    unlockedLevel = 3;
    completedLevels = 3;
    updateButtonStates();
    update();
}

void LevelSelectWindow::setContinueMode(bool enabled)
{
    // here we refresh the doors after toggling between normal and continue mode
    continueMode = enabled;

    loadSaveData();
    updateButtonStates();
    update();
}

QPixmap LevelSelectWindow::getDoorSprite(int level) const
{
    // here we choose which door image each level should use
    if (continueMode && hasContinue) {
        if (level < continueLevel)
            return doorCompleted;

        if (level == continueLevel)
            return doorAvailable;

        return doorLocked;
    }

    if (level <= completedLevels)
        return doorCompleted;

    if (level <= unlockedLevel)
        return doorAvailable;

    return doorLocked;
}

void LevelSelectWindow::updateButtonStates()
{
    for (int i = 0; i < levelButtons.size(); i++) {
        int level = i + 1;

        // here we make locked doors unclickable, same as how they look
        bool clickable = false;

        if (continueMode && hasContinue) {
            clickable = (level <= continueLevel);
        } else {
            clickable = (level <= unlockedLevel);
        }

        levelButtons[i]->setEnabled(clickable);

        levelButtons[i]->setCursor(clickable
                                       ? Qt::PointingHandCursor
                                       : Qt::ForbiddenCursor);
    }
}


void LevelSelectWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // here we keep pixel art crisp by avoiding smoothing on the sprite sheet
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

    QPixmap panel = sheet.copy(PANEL_SRC_X,
                               PANEL_SRC_Y,
                               PANEL_SRC_W,
                               PANEL_SRC_H);

    // here we draw the level select panel first, then draw doors and text on top
    painter.drawPixmap(0,
                       0,
                       PANEL_SRC_W * SCALE,
                       PANEL_SRC_H * SCALE,
                       panel);

    if (!pixelFontFamily.isEmpty())
        painter.setFont(QFont(pixelFontFamily, 11, QFont::Bold));

    painter.setPen(QColor(17, 11, 0));

    painter.drawText(QRect(0,
                           1 * SCALE,
                           PANEL_SRC_W * SCALE,
                           13 * SCALE),
                     Qt::AlignCenter,
                     "LEVELS");

    static const char *levelNames[] = {
        "Greenfields",
        "Dark Dungeon",
        "The Abyss"
    };

    static const char *LevelNums[] = {
        "1",
        "2",
        "3"
    };

    for (int i = 0; i < 3; i++) {
        int level = i + 1;

        int cx = CIRCLE_CX[i] * SCALE;
        int cy = CIRCLE_CY * SCALE;
        int r = CIRCLE_R * SCALE;

        bool locked = true;

        if (continueMode && hasContinue)
            locked = (level > continueLevel);
        else
            locked = (level > unlockedLevel);

        // here we draw the circle backing behind each door
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBrush(QColor(185, 148, 98));
        painter.setPen(QPen(QColor(96, 57, 40), 3));
        painter.drawEllipse(QPoint(cx, cy), r, r);
        painter.setRenderHint(QPainter::Antialiasing, false);

        QPixmap door = getDoorSprite(level);

        // here we scale the small door sprite to fit inside the circle nicely
        int dh = r * 2 - 28;
        int dw = door.width() * dh / door.height();

        painter.drawPixmap(cx - dw / 2,
                           cy - dh / 2 + 2,
                           dw,
                           dh,
                           door);

        if (locked) {
            // here we darken locked levels so the player can read them as unavailable
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setBrush(QColor(0, 0, 0, 110));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPoint(cx, cy), r, r);
            painter.setRenderHint(QPainter::Antialiasing, false);
        }

        if (!pixelFontFamily.isEmpty())
            painter.setFont(QFont(pixelFontFamily, 9, QFont::Bold));

        painter.setPen(QColor(62, 31, 29));

        painter.drawText(QRect(cx - r - 6,
                               cy - r - 22,
                               r * 2 + 12,
                               20),
                         Qt::AlignCenter,
                         QString("LEVEL %1").arg(LevelNums[i]));

        if (!pixelFontFamily.isEmpty())
            painter.setFont(QFont(pixelFontFamily, 7));

        painter.setPen(locked
                           ? QColor(130, 105, 75)
                           : QColor(62, 31, 29));

        painter.drawText(QRect(cx - r - 6,
                               cy + r + 5,
                               r * 2 + 12,
                               16),
                         Qt::AlignCenter,
                         levelNames[i]);
    }

    if (continueMode && hasContinue) {
        // here we show which level the continue save will load into
        if (!pixelFontFamily.isEmpty())
            painter.setFont(QFont(pixelFontFamily, 7));

        painter.setPen(QColor(30, 90, 30));

        int cy = (CIRCLE_CY + CIRCLE_R + 8) * SCALE;

        painter.drawText(QRect(0,
                               cy,
                               PANEL_SRC_W * SCALE,
                               14),
                         Qt::AlignCenter,
                         QString("~ Level %1 in progress ~").arg(continueLevel));
    }
}
