#include "deathwindow.hpp"
#include <QFont>
#include <QFontDatabase>
#include <QProcess>
#include <QPainter>

DeathWindow::DeathWindow(QWidget *parent)
    : BaseWindow(parent)
{
    setFixedSize(500, 300);
    setWindowTitle("Exit Menu");

    QLabel *label = new QLabel("YOU DIED", this);
    label->setFont(pixelFontFamily);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: black;");

    QPushButton *restartBtn = new QPushButton("RESTART LEVEL", this);
    QPushButton *quitBtn = new QPushButton("QUIT TO DESKTOP", this);

    restartBtn->setFont(pixelFontFamily);
    quitBtn->setFont(pixelFontFamily);

    restartBtn->setMinimumHeight(60);
    quitBtn->setMinimumHeight(60);

    restartBtn->setStyleSheet("background-color: #4CAF50; color: black; border-radius: 10px;");
    quitBtn->setStyleSheet("background-color: #f44336; color: black; border-radius: 10px;");

    mainLayout->addWidget(label);
    mainLayout->addWidget(restartBtn);
    mainLayout->addWidget(quitBtn);

    connect(quitBtn, &QPushButton::clicked, qApp, &QApplication::quit);
    connect(restartBtn, &QPushButton::clicked, this, &DeathWindow::accept);
}
