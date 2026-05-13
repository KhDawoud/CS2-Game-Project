#include <QDialog>
#include <QPixmap>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QFontDatabase>
#include <vector>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include "characterselectscreen.hpp"
#include "characters.hpp"

class StatsPanel : public QWidget
{
    friend class CharacterSelectScreen;

protected:
    QLabel *healthIcons[5];
    QLabel *attackIcons[5];
    QLabel *speedIcons[5];

    QPixmap fullHeart, emptyHeart;
    QPixmap fullSword, emptySword;
    QPixmap fullBoot, emptyBoot;

public:
    StatsPanel(QWidget *parent = nullptr) : QWidget(parent)
    {

        setAttribute(Qt::WA_TranslucentBackground);

        int heartSize = 32;
        int swordSize = 40;
        int speedSize = 40;

        fullHeart = QPixmap(":resources/MenuStatsIcons/heart.png").scaled(heartSize, heartSize, Qt::KeepAspectRatio, Qt::FastTransformation);
        emptyHeart = QPixmap(":resources/MenuStatsIcons/empty_heart.png").scaled(heartSize, heartSize, Qt::KeepAspectRatio, Qt::FastTransformation);

        fullSword = QPixmap(":resources/MenuStatsIcons/sword.png").scaled(swordSize, swordSize, Qt::KeepAspectRatio, Qt::FastTransformation);
        emptySword = QPixmap(":resources/MenuStatsIcons/empty_sword.png").scaled(swordSize, swordSize, Qt::KeepAspectRatio, Qt::FastTransformation);

        fullBoot = QPixmap(":resources/MenuStatsIcons/boot.png").scaled(speedSize, speedSize, Qt::KeepAspectRatio, Qt::FastTransformation);
        emptyBoot = QPixmap(":resources/MenuStatsIcons/empty_boot.png").scaled(speedSize, speedSize, Qt::KeepAspectRatio, Qt::FastTransformation);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(30, 30, 30, 30); // Padding inside the parchment
        mainLayout->setSpacing(15);

        mainLayout->addLayout(createStatBlock("Health", healthIcons));
        mainLayout->addLayout(createStatBlock("Attack", attackIcons));
        mainLayout->addLayout(createStatBlock("Speed", speedIcons));

        mainLayout->addStretch(1);
    }

    QVBoxLayout *createStatBlock(const QString &titleText, QLabel *iconArray[])
    {
        QVBoxLayout *blockLayout = new QVBoxLayout();
        blockLayout->setSpacing(5);

        QLabel *title = new QLabel(titleText, this);
        title->setStyleSheet("background-color: transparent; color: #4a2c11; font-weight: bold; font-size: 24px;");
        title->setAlignment(Qt::AlignLeft);
        blockLayout->addWidget(title);

        QHBoxLayout *iconRow = new QHBoxLayout();
        iconRow->setSpacing(5);

        for (int i = 0; i < 5; ++i)
        {
            iconArray[i] = new QLabel(this);
            iconArray[i]->setStyleSheet("background-color: transparent;");
            iconRow->addWidget(iconArray[i]);
        }
        iconRow->addStretch(1);

        blockLayout->addLayout(iconRow);
        return blockLayout;
    }

    void setStats(int health, int attack, const QString &speedStr)
    {
        int healthOut5 = health / 20;
        int attackOut5 = attack / 10;

        int speedOut5 = 3;
        if (speedStr == "Fast")
            speedOut5 = 4;
        else if (speedStr == "Slow")
            speedOut5 = 2;

        for (int i = 0; i < 5; ++i)
        {
            healthIcons[i]->setPixmap(i < healthOut5 ? fullHeart : emptyHeart);
            attackIcons[i]->setPixmap(i < attackOut5 ? fullSword : emptySword);
            speedIcons[i]->setPixmap(i < speedOut5 ? fullBoot : emptyBoot);
        }
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        QPixmap bg(":resources/ui-elements/window.png");
        painter.drawPixmap(0, 0, width(), height(), bg);
    }
};

CharacterSelectScreen::CharacterSelectScreen(int currentCharIndex, QWidget *parent) : QWidget(parent), currentIndex(currentCharIndex - 1)
{
    if (parent)
    {
        setFixedSize(parent->width(), parent->height());
        move(0, 0);
    }
    else
    {
        setFixedSize(1600, 900);
    }

    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);

    setFocusPolicy(Qt::StrongFocus);

    characterData swordsman(1, "Swordsman", ":resources/player/idling/swordsman_1_idling.png", 12, 100, 10, "Medium");
    characterData wizard(2, "Wizard", ":resources/player/idling/Wizard-idle-spritesheet.png", 12, 100, 20, "Medium");
    characterData skeleton(4, "Skeleton", ":resources/player/idling/skeleton-idlesheet.png", 6, 100, 30, "Medium");
    characters = {swordsman, wizard, skeleton};

    leftCharLabel = new QLabel(this);
    centerCharLabel = new QLabel(this);
    rightCharLabel = new QLabel(this);
    nameLabel = new QLabel(this);
    statsPanel = new StatsPanel(this);

    statsPanel->setFixedSize(300, 420);

    prevButton = new QPushButton("<", this);
    nextButton = new QPushButton(">", this);
    confirmButton = new QPushButton("CONFIRM", this);

    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &CharacterSelectScreen::updateAnimation);
    animTimer->start(200);

    leftIndex = (currentIndex - 1 + characters.size()) % characters.size();
    rightIndex = (currentIndex + 1) % characters.size();

    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/pixelfont.ttf");
    if (fontId == -1)
        return;
    QString pixel = QFontDatabase::applicationFontFamilies(fontId).at(0);

    QFont largeFont(pixel, 24, QFont::Bold);
    QFont smallFont(pixel, 20);
    QFont buttonFont(pixel, 20, QFont::Bold);

    // General Layout
    QVBoxLayout *layout0 = new QVBoxLayout(this);
    layout0->setSpacing(0);
    layout0->setContentsMargins(60, 60, 60, 60);

    QLabel *title = new QLabel("SELECT YOUR HERO", this);
    title->setFont(largeFont);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: red; font-size: 80px; background-color: transparent;");
    layout0->addStretch(1);
    layout0->addWidget(title);

    layout0->addSpacing(20);

    nameLabel->setText(characters[currentIndex].name);
    nameLabel->setFont(smallFont);
    nameLabel->setStyleSheet("color: red; font-size: 60px; background-color: transparent;");
    nameLabel->setAlignment(Qt::AlignCenter);

    // Character Display Layout
    QHBoxLayout *layout_chars = new QHBoxLayout();
    layout_chars->setSpacing(20);

    QWidget *leftBalancer = new QWidget(this);
    leftBalancer->setFixedSize(100, 420);
    leftBalancer->setStyleSheet("background-color: transparent;");
    layout_chars->addWidget(leftBalancer);

    setCharacterData();

    layout_chars->addStretch(1);
    layout_chars->addWidget(leftCharLabel);
    leftCharLabel->setAlignment(Qt::AlignCenter);
    leftCharLabel->setStyleSheet("background-color: transparent;");

    layout_chars->addStretch(1);
    layout_chars->addWidget(centerCharLabel);
    centerCharLabel->setAlignment(Qt::AlignCenter);
    centerCharLabel->setStyleSheet("background-color: transparent;");

    layout_chars->addStretch(1);
    layout_chars->addWidget(rightCharLabel);
    rightCharLabel->setAlignment(Qt::AlignCenter);
    rightCharLabel->setStyleSheet("background-color: transparent;");

    layout_chars->addStretch(1);
    layout_chars->addWidget(statsPanel, 0, Qt::AlignRight);

    QGraphicsOpacityEffect *leftOpacity = new QGraphicsOpacityEffect(this);
    leftOpacity->setOpacity(0.4);
    leftCharLabel->setGraphicsEffect(leftOpacity);

    QGraphicsOpacityEffect *rightOpacity = new QGraphicsOpacityEffect(this);
    rightOpacity->setOpacity(0.4);
    rightCharLabel->setGraphicsEffect(rightOpacity);

    // Buttons Layout
    QHBoxLayout *layout_buttons = new QHBoxLayout();
    layout_buttons->setSpacing(30);

    layout_buttons->addWidget(prevButton);
    layout_buttons->addWidget(confirmButton);
    layout_buttons->addWidget(nextButton);

    prevButton->setFont(buttonFont);
    nextButton->setFont(buttonFont);
    confirmButton->setFont(buttonFont);

    prevButton->setStyleSheet("background-color: transparent; color: white; border-radius: 0px; font-size: 50px;");
    nextButton->setStyleSheet("background-color: transparent; color: white; border-radius: 0px; font-size: 50px;");
    confirmButton->setStyleSheet("background-color: transparent; color: white; border-radius: 0px; font-size: 50px;");

    prevButton->setMinimumHeight(80);
    nextButton->setMinimumHeight(80);
    confirmButton->setMinimumHeight(80);

    prevButton->setMinimumWidth(200);
    nextButton->setMinimumWidth(200);
    confirmButton->setMinimumWidth(200);

    layout0->addLayout(layout_chars);
    layout0->addSpacing(30);
    layout0->addWidget(nameLabel);

    layout0->addSpacing(50);
    layout0->addLayout(layout_buttons);
    layout0->addStretch(1);

    connect(prevButton, &QPushButton::clicked, this, &CharacterSelectScreen::updateDisplay);
    connect(nextButton, &QPushButton::clicked, this, &CharacterSelectScreen::updateDisplay);
    connect(confirmButton, &QPushButton::clicked, this, &CharacterSelectScreen::characterSelected);
}

void CharacterSelectScreen::setCharacterData()
{
    leftCharLabel->setPixmap(getScaledFrame(characters[leftIndex], 0, 350));
    rightCharLabel->setPixmap(getScaledFrame(characters[rightIndex], 0, 350));

    centerCharLabel->setPixmap(getScaledFrame(characters[currentIndex], currentFrame, 550));

    statsPanel->setStats(
        characters[currentIndex].health,
        characters[currentIndex].attack,
        characters[currentIndex].speed);

    nameLabel->setText(characters[currentIndex].name);
}

void CharacterSelectScreen::updateDisplay()
{
    currentFrame = 0;

    if (sender() == prevButton)
    {
        rightIndex = currentIndex;
        currentIndex = leftIndex;
        leftIndex = (currentIndex - 1 + characters.size()) % characters.size();
    }
    else if (sender() == nextButton)
    {
        leftIndex = currentIndex;
        currentIndex = rightIndex;
        rightIndex = (currentIndex + 1) % characters.size();
    }

    setCharacterData();
}

void CharacterSelectScreen::updateAnimation()
{
    centerCharLabel->setPixmap(getScaledFrame(characters[currentIndex], currentFrame, 550));

    currentFrame++;
    if (currentFrame >= characters[currentIndex].frameCount)
    {
        currentFrame = 0;
    }
}

void CharacterSelectScreen::characterSelected()
{
    animTimer->stop();
    emit selectionMade(characters[currentIndex].charIndex);
    deleteLater();
}

QPixmap CharacterSelectScreen::getScaledFrame(const characterData &data, int frameIndex, int targetSize)
{
    double exactWidth = (double)data.idleSheet.width() / data.frameCount;
    int startX = qRound(frameIndex * exactWidth);
    int endX = qRound((frameIndex + 1) * exactWidth);
    int w = endX - startX;

    QPixmap cropped = data.idleSheet.copy(
        startX,
        0,
        w,
        data.frameHeight);

    return cropped.scaled(targetSize, targetSize, Qt::KeepAspectRatio, Qt::FastTransformation);
}