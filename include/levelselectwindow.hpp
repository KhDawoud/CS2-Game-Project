#ifndef LEVELSELECTWINDOW_HPP
#define LEVELSELECTWINDOW_HPP

#include <QWidget>
#include <QPushButton>
#include <QPixmap>
#include <QJsonObject>

class LevelSelectWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LevelSelectWindow(QWidget *parent = nullptr);

    // ── Save system (call these from gameplay code) ──────────────────────────

    // Call when player completes a level
    static void markLevelCompleted(int level);

    // Call on save & exit mid-level
    static void saveContinueState(int level, int health, int mana, int stamina, float posX, float posY, int enemiesKilled, int characternum);

    // Call when player starts fresh (clears the continue slot)
    static void clearContinueState();

    // Getters for loading continue state back into the game
    static int   getContinueLevel();
    static int   getContinueHealth();
    static int   getContinueMana();
    static int   getContinueStamina();
    static float getContinuePosX();
    static float getContinuePosY();
    static int   getContinueEnemiesKilled();
    static int   getContinueCharacterNum();
    // True iff a save exists and was not cleared
    static bool  hasContinueState();

    void setContinueMode(bool enabled);
    void forceUnlockAll();   // unlock all levels regardless of save (used from hub)

signals:
    void levelSelected(int level);   // player clicked a level node
    void continueRequested();        // player clicked continue
    void closeRequested();           // player clicked X or quit
    void resetRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    QVector<QPushButton*> levelButtons;


    void updateButtonStates();

public:
    // JSON helpers — public so free helpers in the .cpp can call readSave()
    static QString savePath();
    static QJsonObject readSave();
    static void writeSave(const QJsonObject &obj);

private:
    void loadSaveData();
    void setupButtons();
    QPixmap getDoorSprite(int level) const;
        bool continueMode = false;
    // Sprites
    QPixmap sheet;
    QPixmap doorAvailable;   // brown solid door  – sheet.copy(88,  162, 17, 22)
    QPixmap doorLocked;      // gray door         – sheet.copy(120, 162, 17, 22)
    QPixmap doorCompleted;   // brown glow door   – sheet.copy(56,  162, 17, 22)

    QString pixelFontFamily;

    // State loaded from JSON on construction
    int  unlockedLevel;    // highest level the player may enter  (1–3)
    int  completedLevels;  // highest level already finished      (0–3)
    bool hasContinue;
    int  continueLevel;

    // ── Sprite-sheet layout constants ───────────────────────────────────────
    static constexpr int SCALE = 4;

    // Panel 1 — blank parchment with green header and X button (no baked circles)
    static constexpr int PANEL_SRC_X = 0;
    static constexpr int PANEL_SRC_Y = 0;
    static constexpr int PANEL_SRC_W = 121;
    static constexpr int PANEL_SRC_H = 130;

    // 3 circle centres, evenly spaced, panel-local coords
    static constexpr int CIRCLE_CX[3] = {30, 60, 91};
    static constexpr int CIRCLE_CY    = 58;   // vertically centred in body
    static constexpr int CIRCLE_R     = 14;

    // QUIT button — drawn as a visible styled button (no sprite underneath in panel 1)
    static constexpr int QUIT_X = 25, QUIT_Y = 112, QUIT_W = 71, QUIT_H = 14;

    // Close (×) button — panel 1 has X icon at top-right (panel-local ~108-118, 2-11)
    static constexpr int CLOSE_X = 107, CLOSE_Y = 2, CLOSE_W = 12, CLOSE_H = 10;
};

#endif // LEVELSELECTWINDOW_HPP
