#include "map2.hpp"
#include <QDebug>
#include <QFile>
#include <QRandomGenerator>
#include <QKeyEvent>
#include <cmath>
#include "slime.hpp"

Map::Map()
{
    ImageLoader();
    LoadMapFromCSV(":resources/map-data/level-1-updated.csv");

    // draw the base grass and path tiles
    DrawMapAndGenerateBaseCollisions();
    // the built in things in the map (like buildings)
    PlaceMapStandardTiles();
    // add grass and stones (non-collidable)
    DrawFieldDecorations();
    AddPlayerandStats();

    // place 40 collidable objects randomly with 1 block of spacing
    DistributeRandomCollidables(40, 8, 13, 1);
    AddEnemysRandomly(15,10);

}

void Map::ImageLoader()
{
    // I fixed basically all the hitboxes

    // ==========================================
    // 1. BASE TILES (Loaded from CSV)
    // ==========================================

    baseTiles[0].load(":resources/map-assets/grass.png");

    // Paths (Walkable)
    baseTiles[2].load(":resources/map-assets/up.png");
    baseTiles[3].load(":resources/map-assets/down.png");
    baseTiles[4].load(":resources/map-assets/left.png");
    baseTiles[5].load(":resources/map-assets/right.png");
    baseTiles[6].load(":resources/map-assets/center.png");
    baseTiles[7].load(":resources/map-assets/up-right.png");
    baseTiles[8].load(":resources/map-assets/right-up.png");
    baseTiles[9].load(":resources/map-assets/left-up.png");
    baseTiles[37].load(":resources/map-assets/right-down.png");

    // ==========================================
    // 2. NON-COLLIDABLE DECORATIONS (Field Pool)
    // ==========================================
    nonCollidableDecoPool.push_back(QPixmap(":resources/map-assets/grass1.png"));
    nonCollidableDecoPool.push_back(QPixmap(":resources/map-assets/grass2.png"));
    nonCollidableDecoPool.push_back(QPixmap(":resources/map-assets/grass3.png"));
    nonCollidableDecoPool.push_back(QPixmap(":resources/map-assets/grass4.png"));
    nonCollidableDecoPool.push_back(QPixmap(":resources/map-assets/grass5.png"));
    nonCollidableDecoPool.push_back(QPixmap(":resources/map-assets/grass6.png"));
    nonCollidableDecoPool.push_back(QPixmap(":resources/map-assets/stone1.png"));
    nonCollidableDecoPool.push_back(QPixmap(":resources/map-assets/stone2.png"));

    // ==========================================
    // 3. COLLIDABLE DECORATIONS (Templates)
    // ==========================================

    // Buildings [Indices 0 - 7]
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/house1.png"), QRectF(10, 77, 137, 70)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/house2.png"), QRectF(12, 45, 99, 65)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/house3.png"), QRectF(10, 70, 140, 60)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/house4.png"), QRectF(23, 70, 102, 88)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/tent1.png"), QRectF(3, 26, 63, 39)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/tent2.png"), QRectF(10, 30, 52, 31)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/tent3.png"), QRectF(2, 30, 60, 33)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/tent4.png"),
         QRectF(0, 32, 60, 32)});
    Tent4.load(":resources/map-assets/tent4.png");


    // Trees & Logs [Indices 8 - 13]
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/TreeBasic1.png"), QRectF(57, 77, 16, 24)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/TreeBasic2.png"), QRectF(26, 43, 13, 19)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/TreeBasic3.png"), QRectF(27, 38, 12, 12)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/TreeBasic4.png"), QRectF(58, 74, 11, 21)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/LogBasic1.png"), QRectF(15, 17, 19, 17)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/LogBasic2.png"), QRectF(23, 26, 21, 10)});

    // General Decorations [Indices 14 - 21]
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/cutdownlogs.png"), QRectF(6, 5, 10, 16)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/cart.png"), QRectF(4, 7, 40, 20)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/lamp.png"), QRectF(6, 18, 5, 20)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/axe.png"), QRectF(1, 10, 11, 8)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/barrel.png"), QRectF(0, 0, 20, 22)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/hut.png"), QRectF(5, 26, 33, 27)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/camplog1.png"), QRectF(6, 6, 16, 17)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/camplog2.png"), QRectF(3, 4, 39, 8)});

    // Specific Gates [Indices 22 - 27]
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/topgatel.png"), QRectF(0, 0, 32, 8)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/topgater.png"), QRectF(0, 0, 30, 7)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/bottomgatel.png"), QRectF(0, 0, 1, 30)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/bottomgater.png"), QRectF(31, 5, 1, 30)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/sidegate2"), QRectF(16, 0, 9, 23)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/sidegate3"), QRectF(19, 0, 6, 32)});
    topgatel.load(":resources/map-assets/topgatel.png");
    topgater.load(":resources/map-assets/topgater.png");
    bottomgatel.load(":resources/map-assets/bottomgatel.png");
    bottomgater.load(":resources/map-assets/bottomgater.png");
    sidegate2.load(":resources/map-assets/sidegate2");
    sidegate3.load(":resources/map-assets/sidegate3");

    // ==========================================
    // 4. FORMER CSV WALLS AND GATES [Indices 28+]
    // ==========================================
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/wallt"),
         QRectF(0, 30, 32, 2)}); // these have no hitbox since theyre high up
    collidableTemplates.push_back({QPixmap(":resources/map-assets/wallt2"), QRectF(0, 30, 32, 2)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/wallt3"), QRectF(0, 30, 32, 2)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltb1"), QRectF(0, 0, 31, 26)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltb2"), QRectF(0, 0, 32, 27)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltb3"), QRectF(0, 0, 32, 28)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/Tile2_17.png"), QRectF(6, 0, 10, 32)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/sidewallr2"), QRectF(6, 0, 10, 32)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/Tile2_30.png"), QRectF(15, 0, 10, 32)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/sidewalll2"), QRectF(16, 0, 10, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltr"), QRectF(0, 31, 27, 1)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltrb"), QRectF(6, 8, 21, 24)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltl"), QRectF(5, 30, 27, 2)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltlb"), QRectF(6, 0, 24, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/wallbr"), QRectF(13, 0, 14, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/wallbrb"), QRectF(0, 0, 27, 27)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/sidegate1"), QRectF(16, 0, 10, 32)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/sidegate4"), QRectF(16, 0, 10, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/brokenlt"), QRectF(6, 0, 10, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/brokenlb"), QRectF(6, 0, 6, 27)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/brokent"), QRectF(21, 9, 9, 23)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/brokenb"), QRectF(4, 10, 28, 23)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/brokenlog1"), QRectF(15, 8, 6, 24)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/brokenlog2"), QRectF(10, 0, 8, 20)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/remains1"), QRectF(1, 15, 19, 13)});
    collidableTemplates.push_back(
        {QPixmap(":resources/map-assets/remains2"), QRectF(11, 12, 11, 17)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/remains3"), QRectF(8, 9, 14, 11)});
}

void Map::LoadMapFromCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    mapData.clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(',');
        std::vector<int> row;
        for (const QString &val : values) {
            row.push_back(val.trimmed().toInt());
        }
        mapData.push_back(row);
    }
    file.close();
}

void Map::DrawMapAndGenerateBaseCollisions()
{
    collision_map.assign(MAP_ROWS, std::vector<int>(MAP_COLS, 0));

    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            QGraphicsPixmapItem *base = new QGraphicsPixmapItem(baseTiles[0]);
            base->setPos(j * TILE_SIZE, i * TILE_SIZE);
            base->setZValue(-100.0);
            addItem(base);

            int tileId = mapData[i][j];

            if (tileId != 0 && baseTiles.find(tileId) != baseTiles.end()) {
                QGraphicsPixmapItem *tile = new QGraphicsPixmapItem(baseTiles[tileId]);
                tile->setPos(j * TILE_SIZE, i * TILE_SIZE);
                tile->setZValue(-100.0);
                addItem(tile);
            }

            // for now in this level everything is walkable so no collision map
        }
    }
}

void Map::DrawFieldDecorations()
{
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            if (mapData[i][j] != 0 && mapData[i][j]!=99)
                continue; // only goes on plain grass

            if (QRandomGenerator::global()->bounded(100) < 30) {
                int randomIndex = QRandomGenerator::global()->bounded(
                    static_cast<int>(nonCollidableDecoPool.size()));

                QGraphicsPixmapItem *deco = new QGraphicsPixmapItem(
                    nonCollidableDecoPool[randomIndex]);
                deco->setPos(j * TILE_SIZE, i * TILE_SIZE);

                deco->setZValue(-50.0);
                addItem(deco);
            }
        }
    }
}

void Map::PlaceCollidable(float startRow, float startCol, int templateIndex)
{
    if (templateIndex < 0 || templateIndex >= collidableTemplates.size())
        return;

    const CollidableTemplate &tmpl = collidableTemplates[templateIndex];
    QPointF worldPos(startCol * TILE_SIZE, startRow * TILE_SIZE);
    QGraphicsPixmapItem *entity = new QGraphicsPixmapItem(tmpl.texture);
    entity->setPos(worldPos);

    // this is where it is on the map not the spritesheet
    QRectF worldHitbox = tmpl.localHitbox.translated(worldPos);

    // calculate your z based on where you are on screen so you can walk behind things
    entity->setZValue(worldHitbox.bottom());
    addItem(entity);
    activeCollidableObjects.push_back({worldHitbox});

    // uncomment to check hitboxes
    // QGraphicsRectItem *debugBox = new QGraphicsRectItem(worldHitbox);
    // debugBox->setBrush(QBrush(QColor(255, 0, 0, 100)));
    // debugBox->setPen(QPen(Qt::red));
    // debugBox->setZValue(10000);
    // addItem(debugBox);
}

void Map::DistributeRandomCollidables(int count, int minIndex, int maxIndex, int spacing)
{
    int attempts = 0;
    int placed = 0;
    int maxAttempts = count * 30;

    while (placed < count && attempts < maxAttempts) {
        attempts++;
        int randomRow = QRandomGenerator::global()->bounded(MAP_ROWS);
        int randomCol = QRandomGenerator::global()->bounded(MAP_COLS);

        // choose from the range
        int templateIndex = QRandomGenerator::global()->bounded(minIndex, maxIndex + 1);
        const CollidableTemplate &tmpl = collidableTemplates[templateIndex];

        int widthInTiles = std::ceil((double) tmpl.texture.width() / TILE_SIZE);
        int heightInTiles = std::ceil((double) tmpl.texture.height() / TILE_SIZE);

        if (randomRow + heightInTiles > MAP_ROWS || randomCol + widthInTiles > MAP_COLS) {
            continue;
        }

        bool canPlace = true;

        // only on and around grass
        int checkStartRow = std::max(0, randomRow - spacing);
        int checkEndRow = std::min(MAP_ROWS, randomRow + heightInTiles + spacing);
        int checkStartCol = std::max(0, randomCol - spacing);
        int checkEndCol = std::min(MAP_COLS, randomCol + widthInTiles + spacing);

        for (int r = checkStartRow; r < checkEndRow; r++) {
            for (int c = checkStartCol; c < checkEndCol; c++) {
                if (mapData[r][c] != 0 && mapData[r][c] != 99) {
                    canPlace = false;
                    break;
                }
            }
            if (!canPlace)
                break;
        }

        if (!canPlace)
            continue;

        QRectF visualRect(randomCol * TILE_SIZE,
                          randomRow * TILE_SIZE,
                          tmpl.texture.width(),
                          tmpl.texture.height());

        QRectF spacingRect = visualRect.adjusted(-spacing * TILE_SIZE,
                                                 -spacing * TILE_SIZE,
                                                 spacing * TILE_SIZE,
                                                 spacing * TILE_SIZE);

        QList<QGraphicsItem *> itemsInArea = items(spacingRect);
        for (QGraphicsItem *item : itemsInArea) {
            // no overlapping
            if (item->zValue() > -50.0) {
                canPlace = false;
                break;
            }
        }

        if (canPlace) {
            PlaceCollidable(randomRow, randomCol, templateIndex);
            placed++;
        }
    }
}

void Map::AddCampfire(float startRow, float startCol)
{
    Campfire *cf = new Campfire();
    QPointF worldPos(startCol * TILE_SIZE, startRow * TILE_SIZE);
    cf->setPos(worldPos);

    QRectF worldHitbox(worldPos.x(), worldPos.y() + 16, cf->getwidth(), cf->getheight() - 16);

    cf->setZValue(worldHitbox.bottom());
    addItem(cf);
    activeCampfires.push_back(cf);
}

bool Map::isTileCollidable(int row, int col) const
{
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
        return true;
    return collision_map[row][col] == 1;
}

void Map::PlaceMapStandardTiles()
{
    // cleaned up the issues but we can still decorate the map more

    PlaceCollidable(10.5, 7, 0); // House1
    PlaceCollidable(19.8, 7, 2); // House3
    PlaceCollidable(20, 22, 3);  // House4
    PlaceCollidable(22, 17, 6);  // Tent3
    PlaceCollidable(26, 16, 4);  // Tent1

    // Objects and Gates
    NonCollidablePlaceEntity(13, 16, Tent4, 1000); // Tent4
    NonCollidablePlaceEntity(29, 14, topgatel, 1000);
    NonCollidablePlaceEntity(29, 15, topgater, 1000);
    NonCollidablePlaceEntity(30, 14, bottomgatel, 1000);
    NonCollidablePlaceEntity(30, 15, bottomgater, 1000);
    NonCollidablePlaceEntity(10, 34, sidegate2, 1000);
    NonCollidablePlaceEntity(11, 34, sidegate3, 1000);

    // General Decorations
    PlaceCollidable(14, 12, 15); // Cart
    PlaceCollidable(23.2, 13, 14); // CutDownLogs
    PlaceCollidable(23.2, 12, 17); // Axe
    PlaceCollidable(14.3, 17, 18); // Barrel
    //PlaceCollidable(14, 9, 18); // Jo

    // Campfire Area 2
    PlaceCollidable(28, 10, 20);     // CampLog1
    PlaceCollidable(28.5, 11.3, 21); // CampLog2
    AddCampfire(27, 11);             // Campfire1
    PlaceCollidable(20, 28, 20);     // CampLog1
    PlaceCollidable(20.5, 29.3, 21); // CampLog2
    AddCampfire(19, 29);             // Campfire2

    // Walls
    PlaceCollidable(3, 4, 40);   // ID 22
    PlaceCollidable(3, 5, 28);   // ID 10
    PlaceCollidable(3, 6, 29);   // ID 11
    PlaceCollidable(3, 7, 30);   // ID 12
    PlaceCollidable(3, 8, 28);   // ID 10
    PlaceCollidable(3, 9, 29);   // ID 11
    PlaceCollidable(3, 10, 30);  // ID 12
    PlaceCollidable(3, 11, 28);  // ID 10
    PlaceCollidable(3, 12, 29);  // ID 11
    PlaceCollidable(3, 13, 30);  // ID 12
    PlaceCollidable(3, 14, 28);  // ID 10
    PlaceCollidable(3, 15, 29);  // ID 11
    PlaceCollidable(3, 16, 30);  // ID 12
    PlaceCollidable(3, 17, 28);  // ID 10
    PlaceCollidable(3, 18, 29);  // ID 11
    PlaceCollidable(3, 19, 30);  // ID 12
    PlaceCollidable(3, 20, 28);  // ID 10
    PlaceCollidable(3, 21, 29);  // ID 11
    PlaceCollidable(3, 22, 30);  // ID 12
    PlaceCollidable(3, 23, 28);  // ID 10
    PlaceCollidable(3, 24, 29);  // ID 11
    PlaceCollidable(3, 25, 30);  // ID 12
    PlaceCollidable(3, 26, 28);  // ID 10
    PlaceCollidable(3, 27, 29);  // ID 11
    PlaceCollidable(3, 28, 30);  // ID 12
    PlaceCollidable(3, 29, 28);  // ID 10
    PlaceCollidable(3, 30, 29);  // ID 11
    PlaceCollidable(3, 31, 30);  // ID 12
    PlaceCollidable(3, 32, 28);  // ID 10
    PlaceCollidable(3, 33, 29);  // ID 11
    PlaceCollidable(3, 34, 38);  // ID 20
    PlaceCollidable(4, 4, 41);   // ID 23
    PlaceCollidable(4, 5, 31);   // ID 13
    PlaceCollidable(4, 6, 32);   // ID 14
    PlaceCollidable(4, 7, 33);   // ID 15
    PlaceCollidable(4, 8, 31);   // ID 13
    PlaceCollidable(4, 9, 32);   // ID 14
    PlaceCollidable(4, 10, 33);  // ID 15
    PlaceCollidable(4, 11, 31);  // ID 13
    PlaceCollidable(4, 12, 32);  // ID 14
    PlaceCollidable(4, 13, 33);  // ID 15
    PlaceCollidable(4, 14, 31);  // ID 13
    PlaceCollidable(4, 15, 32);  // ID 14
    PlaceCollidable(4, 16, 33);  // ID 15
    PlaceCollidable(4, 17, 31);  // ID 13
    PlaceCollidable(4, 18, 32);  // ID 14
    PlaceCollidable(4, 19, 33);  // ID 15
    PlaceCollidable(4, 20, 31);  // ID 13
    PlaceCollidable(4, 21, 32);  // ID 14
    PlaceCollidable(4, 22, 33);  // ID 15
    PlaceCollidable(4, 23, 31);  // ID 13
    PlaceCollidable(4, 24, 32);  // ID 14
    PlaceCollidable(4, 25, 33);  // ID 15
    PlaceCollidable(4, 26, 31);  // ID 13
    PlaceCollidable(4, 27, 32);  // ID 14
    PlaceCollidable(4, 28, 33);  // ID 15
    PlaceCollidable(4, 29, 31);  // ID 13
    PlaceCollidable(4, 30, 32);  // ID 14
    PlaceCollidable(4, 31, 33);  // ID 15
    PlaceCollidable(4, 32, 31);  // ID 13
    PlaceCollidable(4, 33, 32);  // ID 14
    PlaceCollidable(4, 34, 39);  // ID 21
    PlaceCollidable(5, 4, 34);   // ID 16
    PlaceCollidable(5, 34, 36);  // ID 18
    PlaceCollidable(6, 4, 34);   // ID 16
    PlaceCollidable(6, 34, 36);  // ID 18
    PlaceCollidable(7, 4, 35);   // ID 17
    PlaceCollidable(7, 34, 36);  // ID 18
    PlaceCollidable(8, 4, 34);   // ID 16
    PlaceCollidable(8, 34, 37);  // ID 19
    PlaceCollidable(9, 4, 34);   // ID 16
    PlaceCollidable(9, 34, 44);  // ID 28
    PlaceCollidable(10, 4, 34);  // ID 16
    PlaceCollidable(11, 4, 35);  // ID 17
    PlaceCollidable(12, 4, 34);  // ID 16
    PlaceCollidable(12, 34, 45); // ID 29
    PlaceCollidable(13, 4, 35);  // ID 17
    PlaceCollidable(13, 34, 36); // ID 18
    PlaceCollidable(14, 4, 34);  // ID 16
    PlaceCollidable(14, 34, 36); // ID 18
    PlaceCollidable(15, 4, 34);  // ID 16
    PlaceCollidable(15, 34, 37); // ID 19
    PlaceCollidable(16, 4, 34);  // ID 16
    PlaceCollidable(16, 34, 36); // ID 18
    PlaceCollidable(17, 4, 34);  // ID 16
    PlaceCollidable(17, 34, 36); // ID 18
    PlaceCollidable(18, 4, 34);  // ID 16
    PlaceCollidable(18, 34, 36); // ID 18
    PlaceCollidable(19, 4, 34);  // ID 16
    PlaceCollidable(19, 34, 37); // ID 19
    PlaceCollidable(20, 4, 34);  // ID 16
    PlaceCollidable(20, 34, 36); // ID 18
    PlaceCollidable(21, 4, 34);  // ID 16
    PlaceCollidable(21, 34, 36); // ID 18
    PlaceCollidable(22, 4, 34);  // ID 16
    PlaceCollidable(22, 34, 37); // ID 19
    PlaceCollidable(23, 4, 35);  // ID 17
    PlaceCollidable(23, 34, 36); // ID 18
    PlaceCollidable(24, 4, 34);  // ID 16
    PlaceCollidable(24, 34, 36); // ID 18
    PlaceCollidable(25, 4, 46);  // ID 24
    PlaceCollidable(25, 34, 36); // ID 18
    PlaceCollidable(26, 4, 47);  // ID 25
    PlaceCollidable(26, 34, 37); // ID 19
    PlaceCollidable(27, 4, 50);  // ID 32
    PlaceCollidable(27, 34, 36); // ID 18
    PlaceCollidable(28, 4, 51);  // ID 33
    PlaceCollidable(28, 6, 54);  // ID 36
    PlaceCollidable(28, 34, 36); // ID 18
    PlaceCollidable(29, 8, 48);  // ID 30
    PlaceCollidable(29, 9, 29);  // ID 11
    PlaceCollidable(29, 10, 30); // ID 12
    PlaceCollidable(29, 11, 28); // ID 10
    PlaceCollidable(29, 12, 29); // ID 11
    PlaceCollidable(29, 13, 30); // ID 12
    PlaceCollidable(29, 16, 30); // ID 12
    PlaceCollidable(29, 17, 28); // ID 10
    PlaceCollidable(29, 18, 29); // ID 11
    PlaceCollidable(29, 19, 30); // ID 12
    PlaceCollidable(29, 20, 28); // ID 10
    PlaceCollidable(29, 21, 29); // ID 11
    PlaceCollidable(29, 22, 30); // ID 12
    PlaceCollidable(29, 23, 28); // ID 10
    PlaceCollidable(29, 24, 29); // ID 11
    PlaceCollidable(29, 25, 30); // ID 12
    PlaceCollidable(29, 26, 28); // ID 10
    PlaceCollidable(29, 27, 29); // ID 11
    PlaceCollidable(29, 28, 30); // ID 12
    PlaceCollidable(29, 29, 28); // ID 10
    PlaceCollidable(29, 30, 29); // ID 11
    PlaceCollidable(29, 31, 30); // ID 12
    PlaceCollidable(29, 32, 28); // ID 10
    PlaceCollidable(29, 33, 29); // ID 11
    PlaceCollidable(29, 34, 42); // ID 26
    PlaceCollidable(30, 5, 52);  // ID 34
    PlaceCollidable(30, 7, 53);  // ID 35
    PlaceCollidable(30, 8, 49);  // ID 31
    PlaceCollidable(30, 9, 32);  // ID 14
    PlaceCollidable(30, 10, 33); // ID 15
    PlaceCollidable(30, 11, 31); // ID 13
    PlaceCollidable(30, 12, 32); // ID 14
    PlaceCollidable(30, 13, 33); // ID 15
    PlaceCollidable(30, 16, 31); // ID 13
    PlaceCollidable(30, 17, 32); // ID 14
    PlaceCollidable(30, 18, 33); // ID 15
    PlaceCollidable(30, 19, 31); // ID 13
    PlaceCollidable(30, 20, 32); // ID 14
    PlaceCollidable(30, 21, 33); // ID 15
    PlaceCollidable(30, 22, 31); // ID 13
    PlaceCollidable(30, 23, 32); // ID 14
    PlaceCollidable(30, 24, 33); // ID 15
    PlaceCollidable(30, 25, 31); // ID 13
    PlaceCollidable(30, 26, 32); // ID 14
    PlaceCollidable(30, 27, 33); // ID 15
    PlaceCollidable(30, 28, 31); // ID 13
    PlaceCollidable(30, 29, 32); // ID 14
    PlaceCollidable(30, 30, 33); // ID 15
    PlaceCollidable(30, 31, 31); // ID 13
    PlaceCollidable(30, 32, 32); // ID 14
    PlaceCollidable(30, 33, 33); // ID 15
    PlaceCollidable(30, 34, 43); // ID 27
}

// to help us create the map
void Map::DrawDebugGridCoordinates()
{
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            QString coordText = QString("%1,%2").arg(i).arg(j);
            QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem(coordText);
            textItem->setPos((j * TILE_SIZE) + 2, (i * TILE_SIZE) + 2);
            textItem->setZValue(20000);
            QFont font = textItem->font();
            font.setPointSize(8);
            font.setBold(true);
            textItem->setFont(font);
            textItem->setBrush(QBrush(Qt::yellow));

            addItem(textItem);
        }
    }
}
void Map::NonCollidablePlaceEntity(float startRow,
                                   float startCol,
                                   const QPixmap &image,
                                   qreal zValue)
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
                mapData[i][j] = 99;
            }
        }
    }
}
void Map::AddEnemysRandomly(int count, int spacing)
{
    int placed = 0;
    int attempts = 0;
    int maxAttempts = count * 100;

    while (placed < count && attempts < maxAttempts) {
        attempts++;
        int randomRow = QRandomGenerator::global()->bounded(5,28);
        int randomCol = QRandomGenerator::global()->bounded(5,33);

        int tileValue = mapData[randomRow][randomCol];
        bool canPlace = (tileValue == 0 || tileValue == 99);

        if (canPlace) {
            int randnum = QRandomGenerator::global()->bounded(100);
            if(randnum<30){
                Slime* slime1 = new Slime();
                slime1->setPos(randomCol * TILE_SIZE, randomRow * TILE_SIZE);
                addItem(slime1);
                slime1->setPlayer(player);
                mapData[randomRow][randomCol] = 98;
                placed++;
                connect(slime1, &Slime::enemyDied, this, &Map::updateEnemyCount);
            }else if(randnum<50){
                Slime *slime2 = new Slime(2);
                slime2->setPos(randomCol * TILE_SIZE, randomRow * TILE_SIZE);
                slime2->setPlayer(player);
                addItem(slime2);
                mapData[randomRow][randomCol] = 98;
                placed++;
                connect(slime2, &Slime::enemyDied, this, &Map::updateEnemyCount);
            }else if(randnum<60){
                Slime *slime3 = new Slime(3);
                slime3->setPos(randomCol * TILE_SIZE, randomRow * TILE_SIZE);
                slime3->setPlayer(player);
                addItem(slime3);
                mapData[randomRow][randomCol] = 98;
                placed++;
                connect(slime3, &Slime::enemyDied, this, &Map::updateEnemyCount);
            }
        }
    }
    currentEnemyCount = placed;

}
void Map::AddPlayerandStats(){
    player = new Player();
    player->setPos(40 * 7, 35 * 28);
    player->setMap(this);
    addItem(player);

    stats = new CharacterStats();
    stats->setPlayer(player);
    stats->setZValue(1000);
    addItem(stats);
    QObject::connect(player, &Player::statsChanged, stats, &CharacterStats::updateBars);
}
Player* Map::getPlayer(){
    return player;
}
CharacterStats* Map::getStats(){
    return stats;
}
void Map::updateEnemyCount() {
    currentEnemyCount--;
    emit requestBarUpdate(currentEnemyCount);

    if (currentEnemyCount <= 0) {
        // here we will add what happens when we win
    }
}
int Map::getCurrentEnimies(){
    return currentEnemyCount;
}
// Use this to test that the bar changes when the slime dies
// void Map::keyPressEvent(QKeyEvent *event) {
//     if (event->key() == Qt::Key_K) {
//         // Find all Slimes in the scene
//         QList<QGraphicsItem*> allItems = items();
//         for (QGraphicsItem* item : allItems) {
//             Slime* slime = dynamic_cast<Slime*>(item);
//             if (slime) {
//                 emit slime->enemyDied(); // Manually trigger the signal
//                 slime->deleteLater(); //remove it from screen too
//                 break;
//             }
//         }
//     }
//     QGraphicsScene::keyPressEvent(event);
// }
