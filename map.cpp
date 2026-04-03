#include "map.h"
#include <vector>
#include <QGraphicsPixmapItem>
#include <QRandomGenerator>

Map::Map(){
    map = {
        // 0: grass 2: up path 3: down path 4: left path 5: right path 6: center path 7: up-right 8:right-up 9:left-up
        // 10: walltop1 11:walltop2 12:walltop3 13: wallbottom1 14: wallbottom2 15: wallbottom3
        //16:sidewallleft1 17:sidewallleft2 18:sidewallgiht1 19:sidewallright2
        //20:walltrt 21:walltrb 22:walltlt 23:walltlb 24:wallblt 25:wallblb 26:walltbrt 27:walltbrb
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 22, 10, 11, 12, 10, 11, 12, 10, 11, 12, 10, 11, 12, 10, 11, 12, 10, 11, 12, 10, 11, 12, 10, 11, 12, 10, 11, 12, 10, 11, 20, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 23, 13, 14, 15, 13, 14, 15, 13, 14, 15, 13, 14, 15, 13, 14, 15, 13, 14, 15, 13, 14, 15, 13, 14, 15, 13, 14, 15, 13, 14, 21, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 7, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 6, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 9, 3, 3, 3, 3, 3, 3, 6, 6, 3, 3, 3, 3, 6, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 7, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 2, 6, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 9, 3, 3, 3, 3, 3, 3, 6, 6, 3, 3, 3, 3, 3, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 24, 10, 11, 12, 10, 11, 12, 10, 11, 12, 4, 5, 12, 10, 11, 12, 10, 11, 12, 10, 11, 12, 10, 11, 12, 10, 11, 12, 10, 11, 26, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 25, 13, 14, 15, 13, 14, 15, 13, 14, 15, 4, 5, 13, 14, 15, 13, 14, 15, 13, 14, 15, 13, 14, 15, 13, 14, 15, 13, 14, 15, 27, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };


    ImageLoader();
    DrawMap();
    AddBuilding(10,7,House1);
    AddBuilding(20,7,House3);
    AddBuilding(6,22,House2);
    AddBuilding(20,22,House4);
    AddObject(13,16,Tent4);
    AddBuilding(8,27,Tent2);
    AddBuilding(22,17,Tent3);
    AddBuilding(26,16,Tent1);
    AddObject(29,14,topgatel);
    AddObject(29,15,topgater);
    AddObject(30,14,bottomgatel);
    AddObject(30,15,bottomgater);
    AddObject(10,34,sidegate2);
    AddObject(11,34,sidegate3);
    DrawField();
}
void Map::DrawMap() {
    int tileSize = 32;

    for(int i = 0; i < 35; i++) {         // i = row (y-axis)
        for(int j = 0; j < 40; j++) {     // j = column (x-axis)
            QGraphicsPixmapItem *base = new QGraphicsPixmapItem(grassPixmap);
            base->setPos(j * tileSize, i * tileSize);
            addItem(base);
            QGraphicsPixmapItem *tile = nullptr;

            switch(map[i][j]){
            case 2:
                tile = new QGraphicsPixmapItem(UpPathPixmap);
                break;
            case 3:
                tile = new QGraphicsPixmapItem(DownPathPixmap);
                break;
            case 4:
                tile = new QGraphicsPixmapItem(LeftPathPixmap);
                break;
            case 5:
                tile = new QGraphicsPixmapItem(RightPathPixmap);
                break;
            case 6:
                tile = new QGraphicsPixmapItem(CenterPathPixmap);
                break;
            case 7:
                tile = new QGraphicsPixmapItem(UpRightPathPixmap);
                break;
            case 8:
                tile = new QGraphicsPixmapItem(RightUpPathPixmap);
                break;
            case 9:
                tile = new QGraphicsPixmapItem(LeftUpPathPixmap);
                break;

            case 10:
                tile = new QGraphicsPixmapItem(walltop1);
                if(i<10){
                    tile->setZValue(0.1);
                }else{
                    tile->setZValue(2.1);
                }
                break;

            case 11:
                tile = new QGraphicsPixmapItem(walltop2);
                if(i<10){
                    tile->setZValue(0.1);
                }else{
                    tile->setZValue(2.1);
                }
                break;
            case 12:
                tile = new QGraphicsPixmapItem(walltop3);
                if(i<10){
                    tile->setZValue(0.1);
                }else{
                    tile->setZValue(2.1);
                }
                break;
            case 13:
                tile = new QGraphicsPixmapItem(wallbottom1);
                if(i<10){
                    tile->setZValue(0.1);
                }else{
                    tile->setZValue(2.1);
                }
                break;

            case 14:
                tile = new QGraphicsPixmapItem(wallbottom2);
                if(i<10){
                    tile->setZValue(0.1);
                }else{
                    tile->setZValue(2.1);
                }
                break;
            case 15:
                tile = new QGraphicsPixmapItem(wallbottom3);
                if(i<10){
                    tile->setZValue(0.1);
                }else{
                    tile->setZValue(2.1);
                }
                break;
            case 16:
                tile = new QGraphicsPixmapItem(sidewalll1);
                tile->setZValue(0.1);
                break;
            case 17:
                tile = new QGraphicsPixmapItem(sidewallr2);
                tile->setZValue(0.1);
                break;
            case 18:
                tile = new QGraphicsPixmapItem(sidewallr1);
                tile->setZValue(0.1);
                break;
            case 19:
                tile = new QGraphicsPixmapItem(sidewalll2);
                tile->setZValue(0.1);
                break;
            case 20:
                tile = new QGraphicsPixmapItem(walltrt);
                tile->setZValue(2.1);
                break;
            case 21:
                tile = new QGraphicsPixmapItem(walltrb);
                tile->setZValue(2.1);
                break;
            case 22:
                tile = new QGraphicsPixmapItem(walltlt);
                tile->setZValue(2.1);
                break;
            case 23:
                tile = new QGraphicsPixmapItem(walltlb);
                tile->setZValue(2.1);
                break;
            case 24:
                tile = new QGraphicsPixmapItem(wallblt);
                tile->setZValue(2.1);
                break;
            case 25:
                tile = new QGraphicsPixmapItem(wallblb);
                tile->setZValue(2.1);
                break;
            case 26:
                tile = new QGraphicsPixmapItem(wallbrt);
                tile->setZValue(2.1);
                break;
            case 27:
                tile = new QGraphicsPixmapItem(wallbrb);
                tile->setZValue(2.1);
                break;
            case 28:
                tile = new QGraphicsPixmapItem(sidegate1);
                tile->setZValue(2.1);
                break;
            case 29:
                tile = new QGraphicsPixmapItem(sidegate4);
                tile->setZValue(2.1);
                break;

            }

            if(tile) {
                // Set position based on loop indices multiplied by tile size
                // x = column index * width, y = row index * height
                tile->setPos(j * tileSize, i * tileSize);
                addItem(tile);
            }
        }
    }
}
void Map::AddBuilding(int startRow, int startCol, QPixmap image) {
    int tileSize = 32;

    int widthInTiles = std::ceil((double)image.width() / tileSize);
    int heightInTiles = std::ceil((double)image.height() / tileSize);

    QGraphicsPixmapItem *building = new QGraphicsPixmapItem(image);
    building->setPos(startCol * tileSize, startRow * tileSize);
    building->setZValue(1.0); // Stay above grass
    addItem(building);

    for(int i = startRow; i < startRow + heightInTiles; i++) {
        for(int j = startCol; j < startCol + widthInTiles; j++) {
            // Safety check to stay inside the 35x40 array
            if(i < 35 && j < 40) {
                map[i][j] = 1;
            }
        }
    }
}
void Map::AddObject(int startRow, int startCol, QPixmap image) {
    int tileSize = 32;

    int widthInTiles = std::ceil((double)image.width() / tileSize);
    int heightInTiles = std::ceil((double)image.height() / tileSize);

    QGraphicsPixmapItem *object = new QGraphicsPixmapItem(image);
    object->setPos(startCol * tileSize, startRow * tileSize);
    object->setZValue(2.1); // Stay above grass and player
    addItem(object);


    for(int i = startRow; i < startRow + heightInTiles; i++) {
        for(int j = startCol; j < startCol + widthInTiles; j++) {

            if(i < 35 && j < 40) {
                map[i][j] = 1;
            }
        }
    }
}
void Map::DrawField(){
    int tileSize = 32;

    for(int i = 0; i < 35; i++) {
        for(int j = 0; j < 40; j++) {
            if(map[i][j] == 0) {
                // Roll a number between 0 and 99
                int chance = QRandomGenerator::global()->bounded(200);

                if(chance < 5) {
                    QGraphicsPixmapItem *grass = new QGraphicsPixmapItem(Grass1);
                    grass->setPos(j * tileSize, i * tileSize);
                    grass->setZValue(0.1); // Slightly above base grass
                    addItem(grass);
                }
                else if(chance < 8) {
                    QGraphicsPixmapItem *grass3 = new QGraphicsPixmapItem(Grass3);
                    grass3->setPos(j * tileSize, i * tileSize);
                    grass3->setZValue(0.1);
                    addItem(grass3);
                }
                else if(chance < 11) {
                    QGraphicsPixmapItem *grass4 = new QGraphicsPixmapItem(Grass4);
                    grass4->setPos(j * tileSize, i * tileSize);
                    grass4->setZValue(0.1);
                    addItem(grass4);
                }
                else if(chance < 14) {
                    QGraphicsPixmapItem *grass5 = new QGraphicsPixmapItem(Grass5);
                    grass5->setPos(j * tileSize, i * tileSize);
                    grass5->setZValue(0.1);
                    addItem(grass5);
                }
                else if(chance < 17) {
                    QGraphicsPixmapItem *grass6 = new QGraphicsPixmapItem(Grass6);
                    grass6->setPos(j * tileSize, i * tileSize);
                    grass6->setZValue(0.1);
                    addItem(grass6);
                }
                else if(chance < 18) {
                    QGraphicsPixmapItem *stone1 = new QGraphicsPixmapItem(Stone1);
                    stone1->setPos(j * tileSize, i * tileSize);
                    stone1->setZValue(0.1);
                    addItem(stone1);
                }
                else if(chance < 20) {
                    QGraphicsPixmapItem *stone1 = new QGraphicsPixmapItem(Stone2);
                    stone1->setPos(j * tileSize, i * tileSize);
                    stone1->setZValue(0.1);
                    addItem(stone1);
                }
            }
        }
    }
}
void Map::ImageLoader(){
    grassPixmap.load(":resources/map-assets/grass.png");
    LeftPathPixmap.load(":resources/map-assets/left.png");
    RightPathPixmap.load(":resources/map-assets/right.png");
    UpPathPixmap.load(":resources/map-assets/up.png");
    DownPathPixmap.load(":resources/map-assets/down.png");
    CenterPathPixmap.load(":resources/map-assets/center.png");
    UpRightPathPixmap.load(":resources/map-assets/up-right.png");
    RightUpPathPixmap.load(":resources/map-assets/right-up.png");
    LeftUpPathPixmap.load(":resources/map-assets/left-up.png");
    House1.load(":resources/map-assets/house1.png");
    House2.load(":resources/map-assets/house2.png");
    House3.load(":resources/map-assets/house3.png");
    House4.load(":resources/map-assets/house4.png");
    Tent1.load(":resources/map-assets/tent1.png");
    Tent2.load(":resources/map-assets/tent2.png");
    Tent3.load(":resources/map-assets/tent3.png");
    Tent4.load(":resources/map-assets/tent4.png");
    Grass1.load(":resources/map-assets/grass1.png");
    Grass2.load(":resources/map-assets/grass2.png");
    Grass3.load(":resources/map-assets/grass3.png");
    Grass4.load(":resources/map-assets/grass4.png");
    Grass5.load(":resources/map-assets/grass5.png");
    Grass6.load(":resources/map-assets/grass6.png");
    Stone1.load(":resources/map-assets/stone1.png");
    Stone2.load(":resources/map-assets/stone2.png");
    topgatel.load(":resources/map-assets/topgatel.png");
    topgater.load(":resources/map-assets/topgater.png");
    bottomgatel.load(":resources/map-assets/bottomgatel.png");
    bottomgater.load(":resources/map-assets/bottomgater.png");
    sidewalll1.load(":resources/map-assets/Tile2_17.png");
    sidewallr1.load(":resources/map-assets/Tile2_30.png");
    sidewalll2.load(":resources/map-assets/sidewalll2");
    sidewallr2.load(":resources/map-assets/sidewallr2");
    wallblt.load(":resources/map-assets/wallbl");
    wallblb.load(":resources/map-assets/wallblb");
    wallbrt.load(":resources/map-assets/wallbr");
    wallbrb.load(":resources/map-assets/wallbrb");
    walltop1.load(":resources/map-assets/wallt");
    walltop2.load(":resources/map-assets/wallt2");
    walltop3.load(":resources/map-assets/wallt3");
    wallbottom1.load(":resources/map-assets/walltb1");
    wallbottom2.load(":resources/map-assets/walltb2");
    wallbottom3.load(":resources/map-assets/walltb3");
    walltlt.load(":resources/map-assets/walltl");
    walltlb.load(":resources/map-assets/walltlb");
    walltrt.load(":resources/map-assets/walltr");
    walltrb.load(":resources/map-assets/walltrb");
    sidegate1.load(":resources/map-assets/sidegate1");
    sidegate2.load(":resources/map-assets/sidegate2");
    sidegate3.load(":resources/map-assets/sidegate3");
    sidegate4.load(":resources/map-assets/sidegate4");
}
