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
        QString pixel = QFontDatabase::applicationFontFamilies(fontId).at(0);

        setFixedSize(500, 300);
        setWindowTitle("Exit Menu");
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);

        QFont largeFont(pixel, 18, QFont::Bold);
        QFont buttonFont(pixel, 14);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(50, 50, 50, 50);
        layout->setSpacing(20);

        QLabel *label = new QLabel("LEVEL SUCCESFULLY CLEARED!", this);
        label->setFont(largeFont);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: black;");

        QPushButton *nextLevelBtn = new QPushButton("RETURN TO HOME", this);
        QPushButton *exit = new QPushButton("SAVE & EXIT", this);

        nextLevelBtn->setFont(buttonFont);
        nextLevelBtn->setMinimumHeight(60);
        nextLevelBtn->setStyleSheet("background-color: #4CAF50; color: black; border-radius: 10px;");

        exit->setFont(buttonFont);
        exit->setMinimumHeight(60);
        exit->setStyleSheet("background-color: #f44336; color: black; border-radius: 10px;");

        layout->addWidget(label);
        layout->addWidget(nextLevelBtn);
        layout->addWidget(exit);

        connect(nextLevelBtn, &QPushButton::clicked, this, &LevelCleared::accept);
        connect(exit, &QPushButton::clicked, qApp, &QApplication::quit);
    }
}
void LevelCleared::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap background(":resources/ui-elements/window.png");
    painter.drawPixmap(0, 0, width(), height(), background);
}
