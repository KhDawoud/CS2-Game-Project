#include "levelintro.hpp"
#include <QFont>
#include <QFontDatabase>

LevelIntro::LevelIntro(QWidget *parent)
    : QDialog(parent)
{
    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/pixelfont.ttf");

    if (fontId != -1)
    {
        QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);

        // this is a bigger window than most since there is a lot of text
        setFixedSize(700, 500);
        setWindowTitle("Exit Menu");
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

        QFont largeFont(family, 18, QFont::Bold);
        QFont smallFont(family, 14, QFont::Bold);
        QFont buttonFont(family, 14);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(30, 30, 30, 30); // Add padding around edges
        layout->setSpacing(20);                     // Space between buttons

        QLabel *label1 = new QLabel("WELCOME TO DEBUG AND DRAGONS", this);
        label1->setFont(largeFont);
        label1->setAlignment(Qt::AlignCenter);

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

        QPushButton *startGameBtn = new QPushButton("Embark on this journey", this);

        startGameBtn->setFont(buttonFont);

        startGameBtn->setMinimumHeight(60); // Make buttons tall

        startGameBtn->setStyleSheet("background-color: #4CAF50; color: white; border-radius: 10px;");

        layout->addWidget(label1);
        layout->addWidget(label2);
        layout->addWidget(startGameBtn);

        connect(startGameBtn, &QPushButton::clicked, this, &QDialog::accept);
    }
}
