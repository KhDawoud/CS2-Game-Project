#include "characters.hpp"

Characters::Characters(int num): Player(num){
    if(num ==1){
        loadSwordsman();
    }else if(num==2){
        loadWizard();
    }else if(num==3){
        loadVampire();
    }else if(num==4){
        loadSkeleton();
    }
}
void Characters::loadSwordsman(){
    damage = 10;
    animationfactor=1;

    walkSheet.load(":resources/player/running/swordsman_1_run.png");
    idleSheet.load(":resources/player/idling/swordsman_1_idle.png");
    attackSheet.load(":resources/player/attacking/swordsman_1_attack.png");
    damagedSheet.load(":resources/player/damaged/swordsman_1_damaged.png");
    deadSheet.load(":resources/player/dead/swordsman_1_dead.png");

    walkFrameWidthCount = 8;
    walkFrameWidth = walkSheet.width() / 8;
    walkFrameHeight = walkSheet.height() / 4;

    idleFrameWidthCount =12;
    idleFrameUpCount =4;
    idleFrameWidth = idleSheet.width() / 12;
    idleFrameHeight = idleSheet.height() / 4;

    attackFrameWidthCount = 8;
    attackFrameWidth = attackSheet.width() / 8;
    attackFrameHeight = attackSheet.height() / 4;

    damagedFrameWidthCount = 5;
    damagedFrameWidth = damagedSheet.width() / 5;
    damagedFrameHeight = damagedSheet.height() / 4;

    deadFrameWidthCount = 7;
    deadFrameWidth = deadSheet.width() / 7;
    deadFrameHeight = deadSheet.height() / 4;

    rowMap[(int)Direction::Up]    = 3;
    rowMap[(int)Direction::Down]  = 0;
    rowMap[(int)Direction::Left]  = 1;
    rowMap[(int)Direction::Right] = 2;

}

void Characters::loadWizard(){
    damage = 20;
    animationfactor = 2;

    walkSheet.load(":resources/player/running/Wizard-walk-spritesheet.png");
    idleSheet.load(":resources/player/idling/Wizard-idle-spritesheet.png");
    attackSheet.load(":resources/player/attacking/Wizard-attack-spritesheet.png");
    damagedSheet.load(":resources/player/damaged/Wizard-hurt-spritesheet.png");
    deadSheet.load(":resources/player/dead/swordsman_1_dead.png"); //for now

    walkFrameWidthCount = 4;
    walkFrameWidth = walkSheet.width() / 4;
    walkFrameHeight = walkSheet.height() / 4;

    idleFrameWidthCount =12;
    idleFrameUpCount=4;
    idleFrameWidth = idleSheet.width() / 12;
    idleFrameHeight = idleSheet.height() / 4;

    attackFrameWidthCount = 7;
    attackFrameWidth = attackSheet.width() / 7;
    attackFrameHeight = attackSheet.height() / 4;

    damagedFrameWidthCount = 4;
    damagedFrameWidth = damagedSheet.width() / 4;
    damagedFrameHeight = damagedSheet.height() / 4;

    deadFrameWidthCount = 7;
    deadFrameWidth = deadSheet.width() / 7;
    deadFrameHeight = deadSheet.height() / 4;

    rowMap[(int)Direction::Up]    = 3;
    rowMap[(int)Direction::Down]  = 0;
    rowMap[(int)Direction::Left]  = 1;
    rowMap[(int)Direction::Right] = 2;

}
void Characters::loadVampire(){
    damage = 30;
    animationfactor = 1;

    walkSheet.load(":resources/player/running/Vampires3_Run_with_shadow.png");
    idleSheet.load(":resources/player/idling/Vampires3_Idle_with_shadow.png");
    attackSheet.load(":resources/player/attacking/Vampires3_Attack_with_shadow.png");
    damagedSheet.load(":resources/player/damaged/Vampires3_Hurt_with_shadow.png");
    deadSheet.load(":resources/player/dead/Vampires3_Death_with_shadow.png");

    walkFrameWidthCount = 8;
    walkFrameWidth = walkSheet.width() / 8;
    walkFrameHeight = walkSheet.height() / 4;

    idleFrameWidthCount =4;
    idleFrameWidth = idleSheet.width() / 4;
    idleFrameHeight = idleSheet.height() / 4;
    idleFrameUpCount=4;

    attackFrameWidthCount = 12;
    attackFrameWidth = attackSheet.width() / 12;
    attackFrameHeight = attackSheet.height() / 4;

    damagedFrameWidthCount = 4;
    damagedFrameWidth = damagedSheet.width() / 4;
    damagedFrameHeight = damagedSheet.height() / 4;

    deadFrameWidthCount = 11;
    deadFrameWidth = deadSheet.width() / 11;
    deadFrameHeight = deadSheet.height() / 4;

    rowMap[(int)Direction::Up]    = 1;
    rowMap[(int)Direction::Down]  = 0;
    rowMap[(int)Direction::Left]  = 2;
    rowMap[(int)Direction::Right] = 3;

}
void Characters::loadSkeleton(){
    damage = 30;
    animationfactor = 1;

    walkSheet.load(":resources/player/running/skeleton-walksheet.png");
    idleSheet.load(":resources/player/idling/skeleton-idlesheet.png");
    attackSheet.load(":resources/player/attacking/skeleton-attacksheet.png");
    damagedSheet.load(":resources/player/damaged/skeleton-hurtsheet.png");
    deadSheet.load(":resources/player/dead/swordsman_1_dead.png"); //for now

    walkFrameWidthCount = 6;
    walkFrameWidth = walkSheet.width() / 6;
    walkFrameHeight = walkSheet.height() / 4;

    idleFrameWidthCount =6;
    idleFrameWidth = idleSheet.width() / 6;
    idleFrameHeight = idleSheet.height() / 4;
    idleFrameUpCount=6;

    attackFrameWidthCount = 6;
    attackFrameWidth = attackSheet.width() / 6;
    attackFrameHeight = attackSheet.height() / 4;

    damagedFrameWidthCount = 5;
    damagedFrameWidth = damagedSheet.width() / 5;
    damagedFrameHeight = damagedSheet.height() / 4;

    deadFrameWidthCount = 7;
    deadFrameWidth = deadSheet.width() / 7;
    deadFrameHeight = deadSheet.height() / 4;

    rowMap[(int)Direction::Up]    = 3;
    rowMap[(int)Direction::Down]  = 0;
    rowMap[(int)Direction::Left]  = 1;
    rowMap[(int)Direction::Right] = 2;

}

void Characters::swtichto(int num){
    this->characternum = num;
    if (num == 1) {
        loadSwordsman();
    }else if(num==2){
        loadWizard();
    }else if(num==3){
        loadVampire();
    }else if(num==4){
        loadSkeleton();
    }
    currentFrame = 0;
}


