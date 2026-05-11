#include <QDialog>
#include <QPixmap>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QFontDatabase>
#include <vector>
#include <QGraphicsOpacityEffect>
#include "characterselectscreen.hpp"
#include "characters.hpp"

class StatsPanel : public QWidget {
    friend class CharacterSelectScreen;
protected:
    QLabel *healthIcons[5];
    QLabel *attackIcons[5];
    QLabel *speedIcons[5];

    QPixmap fullHeart, emptyHeart;
    QPixmap fullSword, emptySword;
    QPixmap fullBoot, emptyBoot;

public:
    StatsPanel(QWidget *parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_TranslucentBackground);

        // 1. INCREASED IMAGE SIZE: Changed from 24x24 to 36x36 (Adjust if you want them even bigger!)
        int heartSize = 36;
        int swordSize = 48;
        int speedSize = 48;
        fullHeart = QPixmap(":resources/MenuStatsIcons/heart.png").scaled(heartSize, heartSize, Qt::KeepAspectRatio, Qt::FastTransformation);
        emptyHeart = QPixmap(":resources/MenuStatsIcons/empty_heart.png").scaled(heartSize, heartSize, Qt::KeepAspectRatio, Qt::FastTransformation);

        fullSword = QPixmap(":resources/MenuStatsIcons/sword.png").scaled(swordSize, swordSize, Qt::KeepAspectRatio, Qt::FastTransformation);
        emptySword = QPixmap(":resources/MenuStatsIcons/empty_sword.png").scaled(swordSize, swordSize, Qt::KeepAspectRatio, Qt::FastTransformation);

        fullBoot = QPixmap(":resources/MenuStatsIcons/boot.png").scaled(speedSize, speedSize, Qt::KeepAspectRatio, Qt::FastTransformation);
        emptyBoot = QPixmap(":resources/MenuStatsIcons/empty_boot.png").scaled(speedSize, speedSize, Qt::KeepAspectRatio, Qt::FastTransformation);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(40, 50, 40, 50); // Slightly reduced margins to make room for bigger items
        mainLayout->setSpacing(20); // More space between the different stat blocks

        // We now call createStatBlock instead of row
        mainLayout->addLayout(createStatBlock("Health", healthIcons));
        mainLayout->addLayout(createStatBlock("Attack", attackIcons));
        mainLayout->addLayout(createStatBlock("Speed",  speedIcons));

        mainLayout->addStretch(1); // Push everything to the top so it doesn't float weirdly
    }

    // 2. NEW LAYOUT: Returns a Vertical Layout (Title on top, Icons below)
    QVBoxLayout* createStatBlock(const QString& titleText, QLabel* iconArray[]) {
        QVBoxLayout* blockLayout = new QVBoxLayout();
        blockLayout->setSpacing(5); // Small gap between the title and the icons below it

        QLabel* title = new QLabel(titleText, this);

        // 3. FONT STYLING: Increased to 22px, changed color to a dark leather brown (#4a2c11)
        title->setStyleSheet("background-color: transparent; color: #4a2c11; font-weight: bold; font-size: 22px;");
        title->setAlignment(Qt::AlignLeft);
        blockLayout->addWidget(title);

        // Create the horizontal row for the icons
        QHBoxLayout* iconRow = new QHBoxLayout();
        iconRow->setSpacing(4); // Slightly more space between the bigger icons

        for (int i = 0; i < 5; ++i) {
            iconArray[i] = new QLabel(this);
            iconArray[i]->setStyleSheet("background-color: transparent;");
            iconRow->addWidget(iconArray[i]);
        }
        iconRow->addStretch(1); // Push icons to the left

        blockLayout->addLayout(iconRow); // Add the row of icons under the title

        return blockLayout;
    }

    void setStats(int health, int attack, const QString& speedStr) {
        // Math remains exactly the same!
        int healthOut5 = health / 20;
        int attackOut5 = attack / 10;

        int speedOut5 = 3;
        if (speedStr == "Fast") speedOut5 = 4;
        else if (speedStr == "Slow") speedOut5 = 2;

        for(int i = 0; i < 5; ++i) {
            healthIcons[i]->setPixmap(i < healthOut5 ? fullHeart : emptyHeart);
            attackIcons[i]->setPixmap(i < attackOut5 ? fullSword : emptySword);
            speedIcons[i]->setPixmap(i  < speedOut5  ? fullBoot  : emptyBoot);
        }
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        QPixmap bg(":resources/ui-elements/window.png");
        painter.drawPixmap(0, 0, width(), height(), bg);
    }
};


CharacterSelectScreen::CharacterSelectScreen(int currentCharIndex, QWidget *parent) : QWidget(parent), currentIndex(currentCharIndex - 1)
{
    setStyleSheet("background-color: black;");

    characterData swordsman(1, "Swordsman", ":resources/player/idling/swordsman_1_idling.png", 12, 100, 10, "Medium");
    characterData wizard(2, "Wizard", ":resources/player/idling/Wizard-idle-spritesheet.png", 12, 100, 20, "Medium");
    characterData vampire(3, "Vampire", ":resources/player/idling/Vampires3_Idle_with_shadow.png", 4, 100, 30, "Fast");
    characterData skeleton(4, "Skeleton", ":resources/player/idling/skeleton-idlesheet.png", 6, 100, 30, "Medium");
    // Setting up the characters vector with the character data
    characters = {swordsman, wizard, vampire, skeleton};

    // Initialize the UI elements
    leftCharLabel = new QLabel(this);
    centerCharLabel = new QLabel(this);
    rightCharLabel = new QLabel(this);
    nameLabel = new QLabel(this);
    statsPanel = new StatsPanel(this);
    statsPanel->setFixedSize(350, 400);


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

    QFont largeFont(pixel, 18, QFont::Bold);
    QFont smallFont(pixel, 14);
    QFont buttonFont(pixel, 14, QFont::Bold);

    //General Layout

    QVBoxLayout *layout0 = new QVBoxLayout(this);
    move(0,0);
    layout0->setSpacing(0);
    layout0->setContentsMargins(100, 100, 100, 100);

    QLabel *title = new QLabel("SELECT YOUR HERO", this);
    title->setFont(largeFont);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: red; font-size: 120px");
    layout0->addStretch(1);
    layout0->addWidget(title);

    layout0->addSpacing(10);

    nameLabel->setText(characters[currentIndex].name);
    nameLabel->setFont(smallFont);
    nameLabel->setStyleSheet("color: red; font-size:100px");
    nameLabel->setAlignment(Qt::AlignCenter);
    //layout0->addWidget(nameLabel);

    //layout0->addStretch(1);
    // Character Display Layout
    QHBoxLayout *layout_chars = new QHBoxLayout(this);
    layout_chars->setSpacing(20);

    QWidget *leftBalancer = new QWidget(this);
    leftBalancer->setFixedSize(350, 400); // Must match your statsPanel size
    layout_chars->addWidget(leftBalancer); // Add it to the far left

    setCharacterData();


    layout_chars->addStretch(1);
    layout_chars->addWidget(leftCharLabel);
    leftCharLabel->setAlignment(Qt::AlignCenter);

    layout_chars->addStretch(1);
    layout_chars->addWidget(centerCharLabel);
    centerCharLabel->setAlignment(Qt::AlignCenter);

    layout_chars->addStretch(1);
    layout_chars->addWidget(rightCharLabel);
    rightCharLabel->setAlignment(Qt::AlignCenter);

    layout_chars->addStretch(1);
    layout_chars->addWidget(statsPanel,0,Qt::AlignRight);

    QGraphicsOpacityEffect *leftOpacity = new QGraphicsOpacityEffect(this);
    leftOpacity->setOpacity(0.4); // 40% visible, adjust as needed
    leftCharLabel->setGraphicsEffect(leftOpacity);

    QGraphicsOpacityEffect *rightOpacity = new QGraphicsOpacityEffect(this);
    rightOpacity->setOpacity(0.4);
    rightCharLabel->setGraphicsEffect(rightOpacity);

    // Buttons Layout

    QHBoxLayout *layout_buttons = new QHBoxLayout(this);
    layout_buttons->setSpacing(20);

    layout_buttons->addWidget(prevButton);
    layout_buttons->addWidget(confirmButton);
    layout_buttons->addWidget(nextButton);

    prevButton->setFont(buttonFont);
    nextButton->setFont(buttonFont);
    confirmButton->setFont(buttonFont);

    prevButton->setStyleSheet("background-color: transparent; color: white; border-radius: 0px; font-size:40px");
    nextButton->setStyleSheet("background-color: transparent; color: white; border-radius: 0px; font-size: 40px");
    confirmButton->setStyleSheet("background-color: transparent; color: white; border-radius: 0px; font-size: 40px");


    prevButton->setMinimumHeight(60);
    nextButton->setMinimumHeight(60);
    confirmButton->setMinimumHeight(60);

    prevButton->setMinimumWidth(150);
    nextButton->setMinimumWidth(150);
    confirmButton->setMinimumWidth(150);

    layout0->addLayout(layout_chars);
    layout0->addSpacing(-100);
    layout0->addWidget(nameLabel);

    layout0->addSpacing(80);
    layout0->addLayout(layout_buttons);
    layout0->addStretch(1);


    // Connect button signals to slots
    connect(prevButton, &QPushButton::clicked, this, &CharacterSelectScreen::updateDisplay);
    connect(nextButton, &QPushButton::clicked, this, &CharacterSelectScreen::updateDisplay);
    connect(confirmButton, &QPushButton::clicked, this, &CharacterSelectScreen::characterSelected);

}

void CharacterSelectScreen::setCharacterData()
{
    leftCharLabel->setPixmap(getScaledFrame(characters[leftIndex], 0, 600));
    rightCharLabel->setPixmap(getScaledFrame(characters[rightIndex], 0, 600));

    centerCharLabel->setPixmap(getScaledFrame(characters[currentIndex], currentFrame, 700));

    statsPanel->setStats(
        characters[currentIndex].health,
        characters[currentIndex].attack,
        characters[currentIndex].speed
        );

    nameLabel->setText(characters[currentIndex].name);
}
void CharacterSelectScreen::updateDisplay()
{
    currentFrame = 0;

    if(sender() == prevButton)
    {
        rightIndex = currentIndex;
        currentIndex = leftIndex;
        leftIndex = (currentIndex - 1 + characters.size()) % characters.size();
    }
    else if(sender() == nextButton)
    {
        leftIndex = currentIndex;
        currentIndex = rightIndex;
        rightIndex = (currentIndex + 1) % characters.size();
    }

    setCharacterData();
    
}

void CharacterSelectScreen::updateAnimation()
{
    centerCharLabel->setPixmap(getScaledFrame(characters[currentIndex], currentFrame, 700));

    currentFrame++;
    if (currentFrame >= characters[currentIndex].frameCount)
    {
        currentFrame = 0;
    }
}

void CharacterSelectScreen::characterSelected()
{
    //add does attack animation here if we have time
    animTimer->stop();
    emit selectionMade(characters[currentIndex].charIndex);
    deleteLater();
}

QPixmap CharacterSelectScreen::getScaledFrame(const characterData& data, int frameIndex, int targetSize)
{
    QPixmap cropped = data.idleSheet.copy(
        frameIndex * data.frameWidth,
        0,
        data.frameWidth,
        data.frameHeight
        );

    return cropped.scaled(targetSize, targetSize, Qt::KeepAspectRatio, Qt::FastTransformation);
}
