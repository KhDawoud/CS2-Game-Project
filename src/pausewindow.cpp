#include "pausewindow.hpp"
#include <QFontDatabase>
#include <QFont>

pausewindow::pausewindow(QWidget *parent): QDialog(parent) {
    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/ARCADECLASSIC.TTF");

    if (fontId != -1) {
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
    QLabel *label = new QLabel("Do you want to quit the game?", this);
    label->setFont(largeFont);
    label->setAlignment(Qt::AlignCenter);

    // 4. Large Buttons
    QPushButton *resumeBtn = new QPushButton("RESUME GAME", this);
    QPushButton *quitBtn = new QPushButton("QUIT TO DESKTOP", this);


    resumeBtn->setFont(buttonFont);
    quitBtn->setFont(buttonFont);

    resumeBtn->setMinimumHeight(60); // Make buttons tall
    quitBtn->setMinimumHeight(60);


    resumeBtn->setStyleSheet("background-color: #4CAF50; color: white; border-radius: 10px;");
    quitBtn->setStyleSheet("background-color: #f44336; color: white; border-radius: 10px;");

    layout->addWidget(label);
    layout->addWidget(resumeBtn);
    layout->addWidget(quitBtn);


    connect(quitBtn, &QPushButton::clicked, qApp, &QApplication::quit);
    connect(resumeBtn, &QPushButton::clicked, this, &QDialog::accept);
    }
}
