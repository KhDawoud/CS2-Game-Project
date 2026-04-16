#include "levelcleared.hpp"
#include <QFont>
#include <QFontDatabase>
#include <QPainter>

LevelCleared::LevelCleared(QWidget *parent)
    : QDialog(parent)
{
    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/pixelfont.ttf");

    if (fontId != -1)
    {
        // Retrieve the exact family name assigned by the font file
        QString pixel = QFontDatabase::applicationFontFamilies(fontId).at(0);

        // Set up the window look
        setFixedSize(500, 300);
        setWindowTitle("Exit Menu");
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);

        // 2. Define a Large Font
        QFont largeFont(pixel, 18, QFont::Bold);
        QFont buttonFont(pixel, 14);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(30, 30, 30, 30); // Add padding around edges
        layout->setSpacing(20);                     // Space between buttons

        // 3. Large Label
        QLabel *label = new QLabel("LEVEL SUCCESFULLY CLEARED!", this);
        label->setFont(largeFont);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: black;");

        // 4. Large Buttons
        QPushButton *nextLevelBtn = new QPushButton("RETURN TO HOME", this);

        nextLevelBtn->setFont(buttonFont);

        nextLevelBtn->setMinimumHeight(60); // Make buttons tall

        nextLevelBtn->setStyleSheet("background-color: #4CAF50; color: black; border-radius: 10px;");
        layout->addWidget(label);
        layout->addWidget(nextLevelBtn);

        // for now it just quits but we'll make it go to the house when we add more levels
        connect(nextLevelBtn, &QPushButton::clicked, qApp, &QApplication::quit);
    }
}
void LevelCleared::paintEvent(QPaintEvent *event)
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
