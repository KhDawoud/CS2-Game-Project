#include "map2.hpp"
#include <QFile>
#include <QRandomGenerator>
#include <cmath>
#include <QDebug>

Map::Map()
{
    ImageLoader();
    LoadMapFromCSV(":resources/map-data/level-1-updated.csv");

    // draw the base grass and path tiles
    DrawMapAndGenerateBaseCollisions();

    // add grass and stones (non-collidable)
    DrawFieldDecorations();

    // the built in things in the map (like buildings)
    PlaceMapStandardTiles();

    // place 40 collidable objects randomly with 1 block of spacing
    DistributeRandomCollidables(40, 8, 13, 1);
}

void Map::ImageLoader()
{
    // I had AI split them into the categories so it needs double checking
    // also nearly all hitboxes are placeholders but once they're fixed it should work

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
    collidableTemplates.push_back({QPixmap(":resources/map-assets/house1.png"), QRectF(0, 63, 154, 86)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/house2.png"), QRectF(0, 30, 116, 82)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/house3.png"), QRectF(0, 32, 156, 103)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/house4.png"), QRectF(0, 0, 147, 59)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/tent1.png"), QRectF(0, 32, 64, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/tent2.png"), QRectF(0, 32, 64, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/tent3.png"), QRectF(0, 32, 64, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/tent4.png"), QRectF(0, 32, 64, 32)});

    // Trees & Logs [Indices 8 - 13]
    collidableTemplates.push_back({QPixmap(":resources/map-assets/TreeBasic1.png"), QRectF(32, 96, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/TreeBasic2.png"), QRectF(32, 96, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/TreeBasic3.png"), QRectF(32, 96, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/TreeBasic4.png"), QRectF(32, 96, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/LogBasic1.png"), QRectF(0, 16, 64, 16)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/LogBasic2.png"), QRectF(0, 16, 64, 16)});

    // General Decorations [Indices 14 - 21]
    collidableTemplates.push_back({QPixmap(":resources/map-assets/cutdownlogs.png"), QRectF(0, 16, 64, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/cart.png"), QRectF(0, 16, 64, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/lamp.png"), QRectF(8, 48, 16, 16)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/axe.png"), QRectF(0, 16, 32, 16)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/barrel.png"), QRectF(0, 16, 32, 16)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/hut.png"), QRectF(0, 32, 64, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/camplog1.png"), QRectF(0, 0, 32, 16)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/camplog2.png"), QRectF(0, 0, 32, 16)});

    // Specific Gates [Indices 22 - 27]
    collidableTemplates.push_back({QPixmap(":resources/map-assets/topgatel.png"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/topgater.png"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/bottomgatel.png"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/bottomgater.png"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/sidegate2"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/sidegate3"), QRectF(0, 0, 32, 32)});

    // ==========================================
    // 4. FORMER CSV WALLS AND GATES [Indices 28+]
    // ==========================================
    collidableTemplates.push_back({QPixmap(":resources/map-assets/wallt"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/wallt2"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/wallt3"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltb1"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltb2"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltb3"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/Tile2_17.png"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/sidewallr2"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/Tile2_30.png"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/sidewalll2"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltr"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltrb"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltl"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/walltlb"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/wallbr"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/wallbrb"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/sidegate1"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/sidegate4"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/brokenlt"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/brokenlb"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/brokent"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/brokenb"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/brokenlog1"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/brokenlog2"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/remains1"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/remains2"), QRectF(0, 0, 32, 32)});
    collidableTemplates.push_back({QPixmap(":resources/map-assets/remains3"), QRectF(0, 0, 32, 32)});
}

void Map::LoadMapFromCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    mapData.clear();
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QStringList values = in.readLine().split(',');
        std::vector<int> row;
        for (const QString &val : values)
        {
            row.push_back(val.trimmed().toInt());
        }
        mapData.push_back(row);
    }
    file.close();
}

void Map::DrawMapAndGenerateBaseCollisions()
{
    collision_map.assign(MAP_ROWS, std::vector<int>(MAP_COLS, 0));

    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            QGraphicsPixmapItem *base = new QGraphicsPixmapItem(baseTiles[0]);
            base->setPos(j * TILE_SIZE, i * TILE_SIZE);
            base->setZValue(-100.0);
            addItem(base);

            int tileId = mapData[i][j];

            if (tileId != 0 && baseTiles.find(tileId) != baseTiles.end())
            {
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
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            if (mapData[i][j] != 0)
                continue; // only goes on plain grass

            if (QRandomGenerator::global()->bounded(100) < 30)
            {
                int randomIndex = QRandomGenerator::global()->bounded(static_cast<int>(nonCollidableDecoPool.size()));

                QGraphicsPixmapItem *deco = new QGraphicsPixmapItem(nonCollidableDecoPool[randomIndex]);
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


    //im drawing hitboxes for now to
    QGraphicsRectItem *debugBox = new QGraphicsRectItem(worldHitbox);
    debugBox->setBrush(QBrush(QColor(255, 0, 0, 100)));
    debugBox->setPen(QPen(Qt::red));
    debugBox->setZValue(10000);
    addItem(debugBox);
}

void Map::DistributeRandomCollidables(int count, int minIndex, int maxIndex, int spacing)
{
    int attempts = 0;
    int placed = 0;
    int maxAttempts = count * 30;

    while (placed < count && attempts < maxAttempts)
    {
        attempts++;
        int randomRow = QRandomGenerator::global()->bounded(MAP_ROWS);
        int randomCol = QRandomGenerator::global()->bounded(MAP_COLS);

        // choose from the range
        int templateIndex = QRandomGenerator::global()->bounded(minIndex, maxIndex + 1);
        const CollidableTemplate &tmpl = collidableTemplates[templateIndex];

        int widthInTiles = std::ceil((double)tmpl.texture.width() / TILE_SIZE);
        int heightInTiles = std::ceil((double)tmpl.texture.height() / TILE_SIZE);

        if (randomRow + heightInTiles > MAP_ROWS || randomCol + widthInTiles > MAP_COLS)
        {
            continue;
        }

        bool canPlace = true;

        // only on and around grass
        int checkStartRow = std::max(0, randomRow - spacing);
        int checkEndRow = std::min(MAP_ROWS, randomRow + heightInTiles + spacing);
        int checkStartCol = std::max(0, randomCol - spacing);
        int checkEndCol = std::min(MAP_COLS, randomCol + widthInTiles + spacing);

        for (int r = checkStartRow; r < checkEndRow; r++)
        {
            for (int c = checkStartCol; c < checkEndCol; c++)
            {
                if (mapData[r][c] != 0)
                {
                    canPlace = false;
                    break;
                }
            }
            if (!canPlace)
                break;
        }

        if (!canPlace)
            continue;

        QRectF visualRect(randomCol * TILE_SIZE, randomRow * TILE_SIZE,
                          tmpl.texture.width(), tmpl.texture.height());

        QRectF spacingRect = visualRect.adjusted(
            -spacing * TILE_SIZE, -spacing * TILE_SIZE,
            spacing * TILE_SIZE, spacing * TILE_SIZE);

        QList<QGraphicsItem *> itemsInArea = items(spacingRect);
        for (QGraphicsItem *item : itemsInArea)
        {
            // no overlapping
            if (item->zValue() > -50.0)
            {
                canPlace = false;
                break;
            }
        }

        if (canPlace)
        {
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

    activeCollidableObjects.push_back({worldHitbox});
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
    // I had AI translate the walls and buildings and stuff and they're not
    // 100% off so we need to remove and fix the wrong ones

    PlaceCollidable(10.5, 7, 0); // House1
    PlaceCollidable(19.8, 7, 2); // House3
    PlaceCollidable(22, 6.5, 1); // House2
    PlaceCollidable(22, 20, 3);  // House4
    PlaceCollidable(27, 8, 5);   // Tent2
    PlaceCollidable(17, 22, 6);  // Tent3
    PlaceCollidable(16, 26, 4);  // Tent1

    // Objects and Gates
    PlaceCollidable(16, 13, 7);  // Tent4
    PlaceCollidable(14, 29, 22); // topgatel
    PlaceCollidable(15, 29, 23); // topgater
    PlaceCollidable(14, 30, 24); // bottomgatel
    PlaceCollidable(15, 30, 25); // bottomgater
    PlaceCollidable(34, 10, 26); // sidegate2
    PlaceCollidable(34, 11, 27); // sidegate3

    // General Decorations
    PlaceCollidable(12, 14, 15);     // Cart
    PlaceCollidable(12, 23.2, 14);   // CutDownLogs
    PlaceCollidable(13, 23.2, 17);   // Axe
    PlaceCollidable(13.5, 25, 16);   // Lamp
    PlaceCollidable(13.5, 16, 16);   // Lamp
    PlaceCollidable(19.9, 9, 16);    // Lamp
    PlaceCollidable(16.9, 14.4, 18); // Barrel
    PlaceCollidable(14, 6, 19);      // WaterWell (hut)

    // Campfire Area 1
    PlaceCollidable(28, 20, 20);     // CampLog1
    PlaceCollidable(29.3, 20.5, 21); // CampLog2
    AddCampfire(29, 19);             // Campfire

    // Campfire Area 2
    PlaceCollidable(10, 28, 20);     // CampLog1
    PlaceCollidable(11.3, 28.5, 21); // CampLog2
    AddCampfire(11, 27);             // Campfire 2

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