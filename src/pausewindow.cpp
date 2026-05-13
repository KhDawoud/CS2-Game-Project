#include "pausewindow.hpp"
#include "QApplication"

pausewindow::pausewindow(QWidget *parent, bool inLevel) : BaseWindow(parent) {
    setWindowTitle("Exit Menu");
    setFixedSize(500, inLevel ? 380 : 300);

    QFont largeFont(pixelFontFamily, 18, QFont::Bold);
    QFont buttonFont(pixelFontFamily, 14);

    QLabel *label = new QLabel("Do you want to quit?", this);
    label->setFont(largeFont);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: black;");

    QPushButton *resumeBtn = new QPushButton("RESUME GAME",    this);
    QPushButton *quitBtn   = new QPushButton("QUIT TO DESKTOP", this);

    resumeBtn->setFont(buttonFont);
    resumeBtn->setStyleSheet("background-color: #4CAF50; color: black; border-radius: 10px;");
    resumeBtn->setMinimumHeight(60);

    quitBtn->setFont(buttonFont);
    quitBtn->setStyleSheet("background-color: #f44336; color: black; border-radius: 10px;");
    quitBtn->setMinimumHeight(60);

    mainLayout->addWidget(label);
    mainLayout->addWidget(resumeBtn);

    if (inLevel) {
        QPushButton *saveBtn = new QPushButton("SAVE AND QUIT", this);
        saveBtn->setFont(buttonFont);
        saveBtn->setStyleSheet("background-color: #2196F3; color: black; border-radius: 10px;");
        saveBtn->setMinimumHeight(60);
        mainLayout->addWidget(saveBtn);

        connect(saveBtn, &QPushButton::clicked, this, [this]() {
            savedAndQuit = true;
            accept();
        });
    }

    mainLayout->addWidget(quitBtn);

    connect(quitBtn,   &QPushButton::clicked, qApp, &QApplication::quit);
    connect(resumeBtn, &QPushButton::clicked, this, &QDialog::accept);
}
