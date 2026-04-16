#include "deathwindow.hpp"
#include <QFont>
#include <QFontDatabase>
#include <QProcess>
#include <QPainter>

DeathWindow::DeathWindow(QWidget *parent)
    : QDialog(parent)
{
    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/pixelfont.ttf");

    if (fontId != -1) {
        // Retrieve the exact family name assigned by the font file
        QString pixel = QFontDatabase::applicationFontFamilies(fontId).at(0);

        // Set up the window look
        setFixedSize(500, 300);
        setWindowTitle("Exit Menu");
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);

        // 2. Define a Large Font
        QFont largeFont(pixel, 24, QFont::Bold);
        QFont buttonFont(pixel, 14);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setSpacing(20);                     // Space between buttons

        // 3. Large Label
        QLabel *label = new QLabel("YOU DIED", this);
        label->setFont(largeFont);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: black;");

        // 4. Large Buttons
        QPushButton *restartBtn = new QPushButton("RESTART GAME", this);
        QPushButton *quitBtn = new QPushButton("QUIT TO DESKTOP", this);

        restartBtn->setFont(buttonFont);
        quitBtn->setFont(buttonFont);

        restartBtn->setMinimumHeight(60); // Make buttons tall
        quitBtn->setMinimumHeight(60);

        restartBtn->setStyleSheet("background-color: #4CAF50; color: black; border-radius: 10px;");
        quitBtn->setStyleSheet("background-color: #f44336; color: black; border-radius: 10px;");

        layout->setContentsMargins(50, 50, 50, 50);
        layout->addWidget(label);
        layout->addWidget(restartBtn);
        layout->addWidget(quitBtn);

        connect(quitBtn, &QPushButton::clicked, qApp, &QApplication::quit);
        connect(restartBtn, &QPushButton::clicked, this, []() {
            // basically just closes and reopens the program for now
            QString program = QCoreApplication::applicationFilePath();
            QStringList args = qApp->arguments();
            args.takeFirst();
            QProcess::startDetached(program, args);
            qApp->quit();
        });
    }
}
void DeathWindow::paintEvent(QPaintEvent *event)
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
