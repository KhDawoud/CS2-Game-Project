#include "campfire.hpp"

Campfire::Campfire() {
    framesheet.load(":resources/map-assets/campfire.png");
    FrameWidth = framesheet.width() / framecount;
    currentframe=0;
    UpdateAnimation();
    animationTimer = new QTimer (this);
    connect(animationTimer, &QTimer::timeout, this, &Campfire::UpdateAnimation);
    animationTimer->start(125);

}
void Campfire::UpdateAnimation(){
    int x = currentframe * FrameWidth;
    setPixmap(framesheet.copy(x, 0,FrameWidth,framesheet.height()));
    currentframe = (currentframe+1)%framecount;

}
int Campfire::getwidth(){
    return FrameWidth;
}
int Campfire::getheight(){
    return framesheet.height();
}
