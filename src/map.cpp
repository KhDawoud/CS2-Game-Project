#include "map.hpp"
#include <QFile>
#include <QGraphicsPixmapItem>
#include <QRandomGenerator>
#include <algorithm>
#include <vector>

Map::Map()
{
    LoadMapFromCSV(":resources/map-data/level-1.csv");
    ImageLoader();
    DrawMap();

    // Buildings (zValue = 1.0)
    PlaceEntity(10.5, 7,5,4, House1, 1.0);
    PlaceEntity(19.8, 7,3,4, House3, 3.0);
    PlaceEntity(6.5, 22,4,3, House2, 1.9);
    NonCollidablePlaceEntity(20, 22,House4, 2.1);
    PlaceEntityWithOffset(8, 27,2,1, Tent2, 2.1);
    PlaceEntityWithOffset(22, 17,2,1, Tent3, 2.1);
    PlaceEntityWithOffset(26, 16,2,1, Tent1, 2.1,-1);

    // Objects and Gates (zValue = 2.1)
    NonCollidablePlaceEntity(13, 16, Tent4, 2.2);
    NonCollidablePlaceEntity(29, 14, topgatel, 2.1);
    NonCollidablePlaceEntity(29, 15, topgater, 2.1);
    NonCollidablePlaceEntity(30, 14, bottomgatel, 0.1);
    NonCollidablePlaceEntity(30, 15, bottomgater, 0.1);
    NonCollidablePlaceEntity(10, 34, sidegate2, 2.1);
    NonCollidablePlaceEntity(11, 34, sidegate3, 2.1);

    // Decorations
    PlaceEntity(14, 12, 2, 1 ,Cart, 1);
    PlaceEntity(23.2, 12, 2, 1,CutDownLogs, 1);
    PlaceEntity(23.2, 13, 1, 1,Axe, 1);
    Addlamp(25, 13.5,Lamp);
    Addlamp(16, 13.5,Lamp);
    Addlamp(11.1, 22,Lamp);
    PlaceEntity(14.3, 17,1,1, Barrel, 1.9);
    PlaceEntity(6, 14,1,2, WaterWell, 2.1);
    NonCollidablePlaceEntity(20, 28,CampLog1, 1.0);
    NonCollidablePlaceEntity(20.5, 29.3,CampLog2, 1.0);
    AddCampfire(19, 29, CampFire);
    NonCollidablePlaceEntity(28, 10,CampLog1, 1.0);
    NonCollidablePlaceEntity(28.5, 11.3,CampLog2, 1.0);
    AddCampfire(27, 11, CampFire2);

    AddTrees();
    DrawCollisionMap();
    DrawField();
}

void Map::LoadMapFromCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ERROR: Could not open the map file:" << filePath;
        return;
    }
    map.clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(',');

        std::vector<int> row;
        for (const QString &val : values) {
            // need to make to int
            row.push_back(val.trimmed().toInt());
        }
        map.push_back(row);
    }
    file.close();
}

void Map::DrawMap()
{
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            // grass is always at z = 0
            QGraphicsPixmapItem *base = new QGraphicsPixmapItem(tileTextures[0]);
            base->setPos(j * TILE_SIZE, i * TILE_SIZE);
            addItem(base);

            int tileId = map[i][j];

            if (tileId != 0 && tileTextures.find(tileId) != tileTextures.end()) {
                QGraphicsPixmapItem *tile = new QGraphicsPixmapItem(tileTextures[tileId]);
                tile->setPos(j * TILE_SIZE, i * TILE_SIZE);

                if (tileId >= 2 && tileId <= 9) {
                    // paths, z = 0
                    tile->setZValue(0.0);
                } else if (tileId >= 10 && tileId <= 12) {
                    // wall top & bottom z = 0.1
                    tile->setZValue((i < 10) ? 0.1 : 2.1);
                } else if (tileId >= 13 && tileId <= 15) {
                    // wall top & bottom z = 0.1
                    tile->setZValue(0.1);
                } else if (tileId >= 16 && tileId <= 19) {
                    // wall sides, z = 0.1
                    tile->setZValue(0.1);
                } else if (tileId >= 20 && tileId <= 29) {
                    // wall corners, z = 2.1
                    tile->setZValue(2.1);
                } else if (tileId == 30) {
                // wall corners, z = 2.1
                tile->setZValue(2.1);
                }

                addItem(tile);
            }
        }
    }
}
void Map::PlaceEntity(float startRow, float startCol,int colW, int colH, const QPixmap & image,qreal zValue)
{

    QGraphicsPixmapItem *entity = new QGraphicsPixmapItem(image);
    entity->setPos(startCol * TILE_SIZE, startRow * TILE_SIZE);
    entity->setZValue(zValue);
    addItem(entity);

    int gridRow = qRound(startRow);
    int gridCol = qRound(startCol);

    for (int i = 0; i < colH; i++) {
        for (int j = 0; j < colW; j++) {
            int r = gridRow + i;
            int c = gridCol + j;
            if (r >= 0 && r < MAP_ROWS && c >= 0 && c < MAP_COLS) {
                map[r][c] = 1;

            }
        }
    }
}
void Map::PlaceEntityWithOffset(float startRow, float startCol,int colW, int colH, const QPixmap & image,qreal zValue, int offsetcontrol)
{
    int heightInTiles = std::ceil((double) image.height() / TILE_SIZE);

    QGraphicsPixmapItem *entity = new QGraphicsPixmapItem(image);
    entity->setPos(startCol * TILE_SIZE, startRow * TILE_SIZE);
    entity->setZValue(zValue);
    addItem(entity);

    int gridRow = qRound(startRow);
    int gridCol = qRound(startCol);
    int rowOffset = (heightInTiles - colH)+offsetcontrol;

    for (int i = 0; i < colH; i++) {
        for (int j = 0; j < colW; j++) {
            int r = gridRow + i +rowOffset;
            int c = gridCol + j;
            if (r >= 0 && r < MAP_ROWS && c >= 0 && c < MAP_COLS) {
                map[r][c] = 1;

            }
        }
    }
}
void Map::NonCollidablePlaceEntity(float startRow, float startCol,const QPixmap &image, qreal zValue)
{
    int widthInTiles = std::ceil((double) image.width() / TILE_SIZE);
    int heightInTiles = std::ceil((double) image.height() / TILE_SIZE);

    QGraphicsPixmapItem *entity = new QGraphicsPixmapItem(image);
    entity->setPos(startCol * TILE_SIZE, startRow * TILE_SIZE);
    entity->setZValue(zValue);
    addItem(entity);

    for (int i = startRow; i < startRow + heightInTiles; i++) {
        for (int j = startCol; j < startCol + widthInTiles; j++) {
            // IMPORTANT: Check bounds for EVERY tile to prevent crashes
            if (i >= 0 && i < MAP_ROWS && j >= 0 && j < MAP_COLS) {
                map[i][j] = 99;
            }
        }
    }
}
void Map::Addlamp(float startRow, float startCol,const QPixmap &image)
{
    QGraphicsPixmapItem *entity = new QGraphicsPixmapItem(image);
    entity->setPos(startCol * TILE_SIZE, startRow * TILE_SIZE);
    entity->setZValue(2.1);
    addItem(entity);
}
void Map::AddCampfire(float startRow, float startCol, Campfire *cf)
{
    int widthInTiles = std::ceil((double) cf->getwidth() / TILE_SIZE);
    int heightInTiles = std::ceil((double) cf->getheight() / TILE_SIZE);

    cf->setPos(startCol * TILE_SIZE, startRow * TILE_SIZE);
    cf->setZValue(1.0);
    addItem(cf);

    for (int i = startRow; i < startRow + heightInTiles; i++) {
        for (int j = startCol; j < startCol + widthInTiles; j++) {
            // IMPORTANT: Check bounds for EVERY tile to prevent crashes
            if (i >= 0 && i < MAP_ROWS && j >= 0 && j < MAP_COLS) {
                map[i][j] = 99;
            }
        }
    }
}
void Map::DrawField()
{
    // vector of all decorations so we just randomly choose one from the vector
    std::vector<QPixmap *> decoPool = {&Grass1,
                                       &Grass1,
                                       &Grass1,
                                       &Grass1,
                                       &Grass3,
                                       &Grass3,
                                       &Grass4,
                                       &Grass4,
                                       &Grass5,
                                       &Grass5,
                                       &Grass6,
                                       &Grass6,
                                       &Stone1,
                                       &Stone2};

    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            if (map[i][j] == 0 || map[i][j] == 99){
                if (QRandomGenerator::global()->bounded(100) < 30) // 30% chance to spawn decoration
                {
                    // this chooses a random decoration from the vector
                    int randomIndex = QRandomGenerator::global()->bounded(
                        static_cast<int>(decoPool.size()));

                    // place decoration
                    QGraphicsPixmapItem *deco = new QGraphicsPixmapItem(*(decoPool[randomIndex]));
                    deco->setPos(j * TILE_SIZE, i * TILE_SIZE);
                    deco->setZValue(0.1);
                    addItem(deco);
                }
            }
        }
    }
}
void Map::ImageLoader()
{
    // Base Grass
    tileTextures[0].load(":resources/map-assets/grass.png");

    // Paths
    tileTextures[2].load(":resources/map-assets/up.png");
    tileTextures[3].load(":resources/map-assets/down.png");
    tileTextures[4].load(":resources/map-assets/left.png");
    tileTextures[5].load(":resources/map-assets/right.png");
    tileTextures[6].load(":resources/map-assets/center.png");
    tileTextures[7].load(":resources/map-assets/up-right.png");
    tileTextures[8].load(":resources/map-assets/right-up.png");
    tileTextures[9].load(":resources/map-assets/left-up.png");
    tileTextures[37].load(":resources/map-assets/right-down.png");

    // Wall Tops & Bottoms
    tileTextures[10].load(":resources/map-assets/wallt");
    tileTextures[11].load(":resources/map-assets/wallt2");
    tileTextures[12].load(":resources/map-assets/wallt3");
    tileTextures[13].load(":resources/map-assets/walltb1");
    tileTextures[14].load(":resources/map-assets/walltb2");
    tileTextures[15].load(":resources/map-assets/walltb3");

    // Side Walls
    tileTextures[16].load(":resources/map-assets/Tile2_17.png"); // sidewalll1
    tileTextures[17].load(":resources/map-assets/sidewallr2");
    tileTextures[18].load(":resources/map-assets/Tile2_30.png"); // sidewallr1
    tileTextures[19].load(":resources/map-assets/sidewalll2");

    // Wall Corners
    tileTextures[20].load(":resources/map-assets/walltr");
    tileTextures[21].load(":resources/map-assets/walltrb");
    tileTextures[22].load(":resources/map-assets/walltl");
    tileTextures[23].load(":resources/map-assets/walltlb");

    tileTextures[26].load(":resources/map-assets/wallbr");
    tileTextures[27].load(":resources/map-assets/wallbrb");

    // Gates
    tileTextures[28].load(":resources/map-assets/sidegate1");
    tileTextures[29].load(":resources/map-assets/sidegate4");

    // Broken parts of the gate
    tileTextures[24].load(":resources/map-assets/brokenlt");
    tileTextures[25].load(":resources/map-assets/brokenlb");
    tileTextures[30].load(":resources/map-assets/brokent");
    tileTextures[31].load(":resources/map-assets/brokenb");
    tileTextures[32].load(":resources/map-assets/brokenlog1");
    tileTextures[33].load(":resources/map-assets/brokenlog2");
    tileTextures[34].load(":resources/map-assets/remains1");
    tileTextures[35].load(":resources/map-assets/remains2");
    tileTextures[36].load(":resources/map-assets/remains3");

    // Buildings
    House1.load(":resources/map-assets/house1.png");
    House2.load(":resources/map-assets/house2.png");
    House3.load(":resources/map-assets/house3.png");
    House4.load(":resources/map-assets/house4.png");

    Tent1.load(":resources/map-assets/tent1.png");
    Tent2.load(":resources/map-assets/tent2.png");
    Tent3.load(":resources/map-assets/tent3.png");
    Tent4.load(":resources/map-assets/tent4.png");

    // Field Decorations
    Grass1.load(":resources/map-assets/grass1.png");
    Grass2.load(":resources/map-assets/grass2.png");
    Grass3.load(":resources/map-assets/grass3.png");
    Grass4.load(":resources/map-assets/grass4.png");
    Grass5.load(":resources/map-assets/grass5.png");
    Grass6.load(":resources/map-assets/grass6.png");
    Stone1.load(":resources/map-assets/stone1.png");
    Stone2.load(":resources/map-assets/stone2.png");
    Tree2.load(":resources/map-assets/TreeBasic2.png");
    Tree3.load(":resources/map-assets/TreeBasic3.png");
    Log2.load(":resources/map-assets/LogBasic2.png");

    // General Decorations
    CutDownLogs.load(":resources/map-assets/cutdownlogs.png");
    Cart.load(":resources/map-assets/cart.png");
    Lamp.load(":resources/map-assets/lamp.png");
    Axe.load(":resources/map-assets/axe.png");
    Barrel.load(":resources/map-assets/barrel.png");
    WaterWell.load(":resources/map-assets/hut.png");
    CampLog1.load(":resources/map-assets/camplog1.png");
    CampLog2.load(":resources/map-assets/camplog2.png");
    // Specific Gates
    topgatel.load(":resources/map-assets/topgatel.png");
    topgater.load(":resources/map-assets/topgater.png");
    bottomgatel.load(":resources/map-assets/bottomgatel.png");
    bottomgater.load(":resources/map-assets/bottomgater.png");
    sidegate2.load(":resources/map-assets/sidegate2");
    sidegate3.load(":resources/map-assets/sidegate3");
}
void Map::DrawCollisionMap()
{
    collision_map.assign(MAP_ROWS, std::vector<int>(MAP_COLS, 0));
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            if ((map[i][j] > 12 && map[i][j] < 27) || map[i][j] == 1
                || map[i][j] > 30 && map[i][j] < 37) {
                collision_map[i][j] = 1;
            } else {
                collision_map[i][j] = 0;
            }
        }
    }
}
void Map::AddTrees()
{
    // made it like the field where it chooses randomly from many tree sprites
    std::vector<QPixmap *> treePool = {&Tree2, &Tree3,&Log2};
    int spacing = 1; // make it so trees don't spawn too close

    // Iterate through the map
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            if (QRandomGenerator::global()->bounded(100) < 20) // 20% chance
            {
                int randomIndex = QRandomGenerator::global()->bounded(
                    static_cast<int>(treePool.size()));
                QPixmap *selectedTree = treePool[randomIndex];

                int widthInTiles = std::ceil((double) selectedTree->width() / TILE_SIZE);
                int heightInTiles = std::ceil((double) selectedTree->height() / TILE_SIZE);

                // so it can't spawn offscreen
                if (i + heightInTiles > MAP_ROWS || j + widthInTiles > MAP_COLS) {
                    continue;
                }
                int checkStartRow = std::max(0, i - spacing);
                int checkEndRow = std::min(MAP_ROWS, i + heightInTiles + spacing);
                int checkStartCol = std::max(0, j - spacing);
                int checkEndCol = std::min(MAP_COLS, j + widthInTiles + spacing);

                bool empty = true;

                for (int x = checkStartRow; x < checkEndRow; x++) {
                    for (int y = checkStartCol; y < checkEndCol; y++) {
                        if (map[x][y] != 0) {
                            empty = false;
                            break;
                        }
                    }
                    if (!empty)
                        break;
                }

                if (empty) {
                    QGraphicsPixmapItem *tree = new QGraphicsPixmapItem(*selectedTree);
                    tree->setPos(j * TILE_SIZE, i * TILE_SIZE);
                    tree->setZValue(2.0 + (i / 100.0));
                    addItem(tree);

                    int trunkWidth = 2;
                    int trunkHeight = 1;


                    int trunkOffsetX = (widthInTiles - trunkWidth);
                    int trunkOffsetY = heightInTiles - trunkHeight;

                    for (int ty = 0; ty < trunkHeight; ty++) {
                        for (int tx = 0; tx < trunkWidth; tx++) {
                            int targetRow = i + trunkOffsetY + ty;
                            int targetCol = j + trunkOffsetX + tx;

                            if (targetRow < MAP_ROWS && targetCol < MAP_COLS) {
                                map[targetRow][targetCol] = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}
bool Map::isTileCollidable(int row, int col)
{
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
        return true;
    return collision_map[row][col] == 1;
}
