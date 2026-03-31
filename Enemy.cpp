#include <iostream>
#include "Enemy.hpp"
#include <QTimer>
#include <QTransform>

using namespace std;

BaseEnemy::BaseEnemy(int hp, int atk, int def, float spd) {
    health = hp;
    maxHealth = hp;
    attack = atk;
    defense = def;
    speed = spd;

    // In BaseEnemy.cpp Constructor
    QTimer* aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &BaseEnemy::update); // 'update' calls move and animate
    aiTimer->start(100); // Ticks every 0.1 seconds 
}

bool BaseEnemy::isalive() {
    return (health > 0);
}

bool BaseEnemy::isdead() {
    return (health <= 0);
}

void BaseEnemy::Attack(Player& target) {
    int damage = attack - target.defense;
    if (damage < 1) damage = 1;

    target.TakeDamage(damage);
}

void BaseEnemy::TakeDamage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}

void BaseEnemy::updateAnimation() {
    if (Dir.y < 0) {
        currentRow = 1;
    }
    else if (Dir.x != 0) {
        currentRow = 2;
    }
    else {
        currentRow = 0;
    }

    if (Dir.x < 0) // means enemy moving to left
    {
        this->setTransform(QTransform().scale(-1, 1)); // mirror the right one memory saving
        this->setTransformOriginPoint(width / 2, height / 2);
    }
    else {
        this->setTransform(QTransform().scale(1, 1)); // if right meaning positive x so normal
    }

    int xCrop = currentFrame * width; // horizontal starting point of each frame
    int yCrop = currentRow * height; // which row we are in

    this->setPixmap(spriteSheet.copy(xCrop, yCrop, width, height)); // take frames and show in screen
    currentFrame = (currentFrame + 1) % totalFrames; // looping across frame to show animation
}

void BaseEnemy::moveEnemy() {
    // Calculate new position
    float newX = this->x() + (Dir.x * speed);
    float newY = this->y() + (Dir.y * speed);

    // Update the position
    this->setPos(newX, newY);
}

void BaseEnemy::update() {
    if (!isalive()) {
        // If the enemy is dead, don't move or animate
        return;
    }

    moveEnemy();       // 1. Change the X,y
    updateAnimation(); // 2. Change the picture to match movement
}
