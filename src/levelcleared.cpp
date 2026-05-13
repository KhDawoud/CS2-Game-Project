#include "levelcleared.hpp"

LevelCleared::LevelCleared(QWidget *parent, int level)
    : BaseWindow(parent), CurrentLevel(level)
{
    setFixedSize(500,300);
    QFont largeFont(pixelFontFamily, 18, QFont::Bold);
    QFont buttonFont(pixelFontFamily, 14);

    QLabel *label = new QLabel(QString("LEVEL %1 CLEARED!").arg(CurrentLevel), this);
    label->setFont(largeFont);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: black;");

    QPushButton *nextLevelBtn = new QPushButton("RETURN TO HOME", this);
    nextLevelBtn->setFont(buttonFont);
    nextLevelBtn->setMinimumHeight(60);
    nextLevelBtn->setStyleSheet("background-color: #4CAF50; color: black; border-radius: 10px;");

    mainLayout->addWidget(label);
    mainLayout->addWidget(nextLevelBtn);

    connect(nextLevelBtn, &QPushButton::clicked, this, &LevelCleared::accept);
}
