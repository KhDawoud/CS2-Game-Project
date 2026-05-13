#include "basewindow.hpp"

//A base window class that just handles the common styling stuff like the background and the pixel font

BaseWindow::BaseWindow(QWidget *parent) : QDialog(parent) {
    int fontId = QFontDatabase::addApplicationFont(":resources/fonts/pixelfont.ttf");
    if (fontId != -1) {
        pixelFontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0); //sets custom font
    }

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground); //makes it suitable for the loaded background

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50); // so that the contents dont get into the frame of the png
    mainLayout->setSpacing(20);
}

void BaseWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPixmap background(":resources/ui-elements/window.png"); //png of a window background
    painter.drawPixmap(0, 0, width(), height(), background);
}

