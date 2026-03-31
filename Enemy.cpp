#include<iostream>
#include"Enemy.hpp"
#include<QKeyEvent>
using namespace std;
enum class  EnemyState {
    IDLE,
    CHASE,
    ATTACK,
    DEAD
};
struct Position {

  Position(float x = 0.0f, float y = 0.0f) 
{
this->x=x;
this->y=y;
}
  float x, y;
};

struct Direction {
  Direction(float x = 0.0f, float y = 0.0f) 
{
this->x=x;
this->y=y;
}
  float x, y;
};


class baseEnemy{

protected:
Position Pos;
Direction Dir;
int health;
int Maxhealth;
float speed=2.0f;
int attack;
int defense;
int width, height;
int currentFrame;
int currentRow;
QPixmap spriteSheet;
public:
BaseEnemy(int hp, int atk, int def, float spd = 1.0f) {
        health = hp;
        maxHealth = hp;
        attack = atk;
        defense = def;
        speed = spd;
}
bool isalive()
{
return (health>0)
}
bool isdead()
{
return (health<0)
}
void Attack(Player& target) {
    int damage = attack - target.defense;
    if (damage < 1) damage = 1;

    target.TakeDamage(damage);
}
void TakeDamage(int amount) {
        health -= amount;
        if (health < 0) health = 0;
    }
virtual void updateAnimation(){

if (Dir.y < 0) {
        currentRow = 1; 
    } 
    else if (Dir.x != 0) {
        currentRow = 2;
    } 
    else {
        currentRow = 0; 
    }
if(dir.x<0);//means enemy moving to left
{
this->setTransform(QTransform().scale(-1,1));//mirror the right one memory saving
this->setTransformOriginPoint(width/2, height/2);}
else{
this->setTransform(QTransform().scale(1,1));//if right meaning positive x so normal
};

int xCrop = currentFrame * width;//horizontal starting point of each frame
int yCrop = currentRow * height;//which row we are in 

this->setPixmap(spriteSheet.copy(xCrop, yCrop, width, height));//take frames and show in screen
currentFrame = (currentFrame + 1) % totalFrames;// looping across frame to show animation
}
void moveEnemy() {
    // Calculate new position: Current + (Direction * Speed)
    float newX = this->x() + (Dir.x * speed);
    float newY = this->y() + (Dir.y * speed);

    // Update the position in the game scene
    this->setPos(newX, newY);
}
void BaseEnemy::update() {
    if (!isalive()) {
        // If the enemy is dead, don't move or animate
        return; 
    }

    moveEnemy();       // 1. Change the X, Y math
    updateAnimation(); // 2. Change the picture to match movement
}
void BaseEnemy::keyPressEvent(QKeyEvent *event) {
    // 1. Reset direction so we don't slide forever
    Dir.x = 0;
    Dir.y = 0;

    // 2. Set Direction based on the key
    if (event->key() == Qt::Key_W) {
        Dir.y = -1; // Up
    } else if (event->key() == Qt::Key_S) {
        Dir.y = 1;  // Down
    } else if (event->key() == Qt::Key_A) {
 void BaseEnemy::keyPressEvent(QKeyEvent *event) {
    // 1. Reset direction so we don't slide forever
    Dir.x = 0;
    Dir.y = 0;

    // 2. Set Direction based on the key
    if (event->key() == Qt::Key_W) {
        Dir.y = -1; // Up
    } else if (event->key() == Qt::Key_S) {
        Dir.y = 1;  // Down
    } else if (event->key() == Qt::Key_A) {
        Dir.x = -1; // Left
    } else if (event->key() == Qt::Key_D) {
        Dir.x = 1;  // Right
    }

    // 3. MANUAL COMMANDS: Move first, then update look
    moveEnemy();        // Updates X/Y
    updateAnimation();  // Flips and crops the 64x64 frame
}       Dir.x = -1; // Left
    } else if (event->key() == Qt::Key_D) {
        Dir.x = 1;  // Right
    }

    // 3. MANUAL COMMANDS: Move first, then update look
    moveEnemy();        // Updates X/Y
    updateAnimation();  // Flips and crops the 64x64 frame
}
