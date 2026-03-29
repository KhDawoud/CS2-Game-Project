#include<iostream>
using namespace std;
enum  EnemyState {
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
float speed;
int attack;
int defense;
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
};
