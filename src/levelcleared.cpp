#include "levelcleared.hpp"
#include <QFont>
#include <QFontDatabase>

LevelCleared::LevelCleared(QWidget *parent)
    : QDialog(parent)
{
    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/pixelfont.ttf");

    if (fontId != -1)
    {
        // Retrieve the exact family name assigned by the font file
        QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);

        // Set up the window look
        setFixedSize(500, 300);
        setWindowTitle("Exit Menu");
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

        // 2. Define a Large Font
        QFont largeFont(family, 18, QFont::Bold);
        QFont buttonFont(family, 14);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(30, 30, 30, 30); // Add padding around edges
        layout->setSpacing(20);                     // Space between buttons

        // 3. Large Label
        QLabel *label = new QLabel("LEVEL SUCCESFULLY CLEARED!", this);
        label->setFont(largeFont);
        label->setAlignment(Qt::AlignCenter);

        // 4. Large Buttons
        QPushButton *nextLevelBtn = new QPushButton("RETURN TO HOME", this);

        nextLevelBtn->setFont(buttonFont);

        nextLevelBtn->setMinimumHeight(60); // Make buttons tall

        nextLevelBtn->setStyleSheet("background-color: #4CAF50; color: white; border-radius: 10px;");

        layout->addWidget(label);
        layout->addWidget(nextLevelBtn);

        // for now it just quits but we'll make it go to the house when we add more levels
        connect(nextLevelBtn, &QPushButton::clicked, qApp, &QApplication::quit);
    }
}
