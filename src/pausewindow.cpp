#include "pausewindow.hpp"
#include <QFont>
#include <QFontDatabase>
#include <QPainter>

pausewindow::pausewindow(QWidget *parent)
    : QDialog(parent)
{
    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/pixelfont.ttf");

    if (fontId != -1) {
        //sets pixel as a the custom fint we got
        QString pixel = QFontDatabase::applicationFontFamilies(fontId).at(0);

        // the window look
        setFixedSize(500, 300);
        setWindowTitle("Exit Menu");
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);


        QFont largeFont(pixel, 18, QFont::Bold);
        QFont buttonFont(pixel, 14);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(30, 30, 30, 30); // Add padding around edges
        layout->setSpacing(20);                     // Space between buttons


        QLabel *label = new QLabel("Do you want to quit the game?", this);
        label->setFont(largeFont);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: black;");

        QPushButton *resumeBtn = new QPushButton("RESUME GAME", this);
        QPushButton *quitBtn = new QPushButton("QUIT TO DESKTOP", this);

        resumeBtn->setFont(buttonFont);
        quitBtn->setFont(buttonFont);

        resumeBtn->setMinimumHeight(60);
        quitBtn->setMinimumHeight(60);

        resumeBtn->setStyleSheet("background-color: #4CAF50; color: black; border-radius: 10px;");
        quitBtn->setStyleSheet("background-color: #f44336; color: black; border-radius: 10px;");

        layout->setContentsMargins(50, 50, 50, 50);
        layout->addWidget(label);
        layout->addWidget(resumeBtn);
        layout->addWidget(quitBtn);

        connect(quitBtn, &QPushButton::clicked, qApp, &QApplication::quit);
        connect(resumeBtn, &QPushButton::clicked, this, &QDialog::accept);
    }
}
void pausewindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap background(":resources/ui-elements/window.png");
    painter.drawPixmap(0, 0, width(), height(), background);
}
