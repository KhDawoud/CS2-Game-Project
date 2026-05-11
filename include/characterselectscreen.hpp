#ifndef CHARACTERSELECTSCREEN_H
#define CHARACTERSELECTSCREEN_H

#include <QString>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <vector>

struct characterData
{
    int charIndex;
    QString name;
    QPixmap idleSheet;
    int frameWidth;
    int frameHeight;
    int frameCount;
    int health;
    int attack;
    QString speed;

    characterData(int index, QString name, QString idlePath, int frameCount, int health, int attack, QString speed)
        : charIndex(index), name(name), frameCount(frameCount), health(health), attack(attack), speed(speed)
    {
        idleSheet.load(idlePath);
        frameWidth = idleSheet.width() / frameCount;
        frameHeight = idleSheet.height() / 4;
    }
};

class StatsPanel;

class CharacterSelectScreen : public QWidget
{
    Q_OBJECT
public:
    explicit CharacterSelectScreen(int currentCharIndex, QWidget *parent = nullptr);

signals:
    void selectionMade(int charIndex);

protected:
private:
    std::vector<characterData> characters;
    int currentIndex = 0;
    int leftIndex = 0;
    int rightIndex = 0;
    int currentFrame = 0;

    QLabel *leftCharLabel = nullptr;
    QLabel *centerCharLabel = nullptr;
    QLabel *rightCharLabel = nullptr;
    QLabel *nameLabel = nullptr;
    // QLabel *statsLabel = nullptr;
    StatsPanel *statsPanel = nullptr;

    QPushButton *prevButton = nullptr;
    QPushButton *nextButton = nullptr;
    QPushButton *confirmButton = nullptr;
    QTimer *animTimer = nullptr;

    void updateDisplay();
    void updateAnimation();
    void setCharacterData();
    void characterSelected();
    QPixmap getScaledFrame(const characterData &data, int frameIndex, int targetSize);
};

#endif // CHARACTERSELECTSCREEN_H
