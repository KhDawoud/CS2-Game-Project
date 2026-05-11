#include "pausewindow.hpp"
#include "QApplication"

pausewindow::pausewindow(QWidget *parent) : BaseWindow(parent) {
    setWindowTitle("Exit Menu");
    setFixedSize(500,300);

    QFont largeFont(pixelFontFamily, 18, QFont::Bold);
    QFont buttonFont(pixelFontFamily, 14);

    QLabel *label = new QLabel("Do you want to quit?", this);
    label->setFont(largeFont);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: black;");

    QPushButton *resumeBtn = new QPushButton("RESUME GAME", this);
    QPushButton *quitBtn = new QPushButton("QUIT TO DESKTOP", this);

    resumeBtn->setFont(buttonFont);
    resumeBtn->setStyleSheet("background-color: #4CAF50; color: black; border-radius: 10px;");
    resumeBtn->setMinimumHeight(60);

    quitBtn->setFont(buttonFont);
    quitBtn->setStyleSheet("background-color: #f44336; color: black; border-radius: 10px;");
    quitBtn->setMinimumHeight(60);

    mainLayout->addWidget(label);
    mainLayout->addWidget(resumeBtn);
    mainLayout->addWidget(quitBtn);

    connect(quitBtn, &QPushButton::clicked, qApp, &QApplication::quit);
    connect(resumeBtn, &QPushButton::clicked, this, &QDialog::accept);
}
