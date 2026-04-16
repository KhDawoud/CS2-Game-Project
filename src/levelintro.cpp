#include "levelintro.hpp"
#include <QFont>
#include <QFontDatabase>
#include <QPainter>

LevelIntro::LevelIntro(QWidget *parent)
    : QDialog(parent)
{
    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/pixelfont.ttf");

    if (fontId != -1)
    {
        QString pixel = QFontDatabase::applicationFontFamilies(fontId).at(0);

        // this is a bigger window than most since there is a lot of text
        setFixedSize(1000, 700);
        setWindowTitle("Exit Menu");
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);

        QFont largeFont(pixel, 18, QFont::Bold);
        QFont smallFont(pixel, 14, QFont::Bold);
        QFont buttonFont(pixel, 14);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setSpacing(20);                     // Space between buttons

        QLabel *label1 = new QLabel("WELCOME TO DEBUG AND DRAGONS", this);
        label1->setFont(largeFont);
        label1->setAlignment(Qt::AlignCenter);
        label1->setStyleSheet("color: black;");

        QLabel *label2 = new QLabel("You wake up on your floor with a pounding headache and no clue what happened.\n"
                                    "\n"
                                    "\"Hello?\" you call out.\n"
                                    "\n"
                                    "Nothing.\n"
                                    "\n"
                                    "You check the house—empty. No family. No neighbors. Just a weird, unsettling silence.\n"
                                    "\n"
                                    "Then you peek out the window.\n"
                                    "\n"
                                    "Slimes. Everywhere.\n"
                                    "\n"
                                    "They’re bouncing around your village like they own the place.\n"
                                    "\n"
                                    "...Yeah, no. Not happening.\n"
                                    "\n"
                                    "You grab whatever you can find.\n"
                                    "\n"
                                    "Time to clear out some slimes—and figure out where everyone went.\n",
                                    this);
        label2->setFont(smallFont);
        label2->setAlignment(Qt::AlignCenter);
        label2->setStyleSheet("color: black;");

        QPushButton *startGameBtn = new QPushButton("Embark on this journey", this);

        startGameBtn->setFont(buttonFont);

        startGameBtn->setMinimumHeight(60);

        startGameBtn->setStyleSheet("background-color: #4CAF50; color: black; border-radius: 10px;");

        layout->setContentsMargins(100, 100, 100, 100); // so that the borders of the png are not intersecting
        layout->addWidget(label1);
        layout->addWidget(label2);
        layout->addWidget(startGameBtn);

        connect(startGameBtn, &QPushButton::clicked, this, &QDialog::accept);
    }
}
void LevelIntro::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Load your PNG
    QPixmap background(":resources/ui-elements/window.png");

    // Draw it to fill the entire 500x300 area
    if (!background.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), background);
    } else {
        // Optional: Draw a solid color if the image fails to load
        painter.fillRect(rect(), QColor(40, 40, 40, 200));
    }
}
