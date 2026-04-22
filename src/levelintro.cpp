#include "levelintro.hpp"
#include <QFont>
#include <QFontDatabase>
#include <QPainter>

LevelIntro::LevelIntro(QWidget *parent)
    : QWidget(parent)
{
    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/pixelfont.ttf");
    if (fontId == -1)
        return;

    QString pixel = QFontDatabase::applicationFontFamilies(fontId).at(0);

    setFixedSize(1000, 700);
    move((parent->width() - 1000) / 2, (parent->height() - 700) / 2);
    setAttribute(Qt::WA_TranslucentBackground);

    QFont largeFont(pixel, 18, QFont::Bold);
    QFont smallFont(pixel, 14, QFont::Bold);
    QFont buttonFont(pixel, 14);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(100, 100, 100, 100);

    QLabel *label1 = new QLabel("WELCOME TO DEBUG AND DRAGONS", this);
    label1->setFont(largeFont);
    label1->setAlignment(Qt::AlignCenter);
    label1->setStyleSheet("color: black;");

    QLabel *label2 = new QLabel(
        "You wake up on your floor with a pounding headache and no clue what happened.\n"
        "\n\"Hello?\" you call out.\n\nNothing.\n\n"
        "You check the house—empty. No family. No neighbors. Just a weird, unsettling silence.\n"
        "\nThen you peek out the window.\n\nSlimes. Everywhere.\n\n"
        "They're bouncing around your village like they own the place.\n\n"
        "...Yeah, no. Not happening.\n\nYou grab whatever you can find.\n\n"
        "Time to clear out some slimes—and figure out where everyone went.\n",
        this);
    label2->setFont(smallFont);
    label2->setAlignment(Qt::AlignCenter);
    label2->setStyleSheet("color: black;");

    QPushButton *startGameBtn = new QPushButton("Embark on this journey", this);
    startGameBtn->setFont(buttonFont);
    startGameBtn->setMinimumHeight(60);
    startGameBtn->setStyleSheet("background-color: #4CAF50; color: black; border-radius: 10px;");

    layout->addWidget(label1);
    layout->addWidget(label2);
    layout->addWidget(startGameBtn);

    connect(startGameBtn, &QPushButton::clicked, this, [this]()
            {
        emit introFinished();
        deleteLater(); });
}

void LevelIntro::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap background(":resources/ui-elements/window.png");
    painter.drawPixmap(0, 0, width(), height(), background);
}