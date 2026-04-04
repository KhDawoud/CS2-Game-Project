#include "map.hpp"
#include <QFile>
#include <vector>
#include <QGraphicsPixmapItem>
#include <QRandomGenerator>

Map::Map()
{
    LoadMapFromCSV(":resources/map-data/level-1.csv");
    ImageLoader();
    DrawMap();

    // Buildings (zValue = 1.0)
    PlaceEntity(10, 7, House1, 1.0);
    PlaceEntity(20, 7, House3, 1.0);
    PlaceEntity(6, 22, House2, 1.0);
    PlaceEntity(20, 22, House4, 1.0);
    PlaceEntity(8, 27, Tent2, 1.0);
    PlaceEntity(22, 17, Tent3, 1.0);
    PlaceEntity(26, 16, Tent1, 1.0);

    // Objects and Gates (zValue = 2.1)
    PlaceEntity(13, 16, Tent4, 2.1);
    PlaceEntity(29, 14, topgatel, 2.1);
    PlaceEntity(29, 15, topgater, 2.1);
    PlaceEntity(30, 14, bottomgatel, 2.1);
    PlaceEntity(30, 15, bottomgater, 2.1);
    PlaceEntity(10, 34, sidegate2, 2.1);
    PlaceEntity(11, 34, sidegate3, 2.1);

    DrawField();
}

void Map::LoadMapFromCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "ERROR: Could not open the map file:" << filePath;
        return;
    }
    map.clear();
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList values = line.split(',');

        std::vector<int> row;
        for (const QString &val : values)
        {
            // need to make to int
            row.push_back(val.trimmed().toInt());
        }
        map.push_back(row);
    }
    file.close();
}

void Map::DrawMap()
{
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            // grass is always at z = 0
            QGraphicsPixmapItem *base = new QGraphicsPixmapItem(tileTextures[0]);
            base->setPos(j * TILE_SIZE, i * TILE_SIZE);
            addItem(base);

            int tileId = map[i][j];

            if (tileId != 0 && tileTextures.find(tileId) != tileTextures.end())
            {
                QGraphicsPixmapItem *tile = new QGraphicsPixmapItem(tileTextures[tileId]);
                tile->setPos(j * TILE_SIZE, i * TILE_SIZE);

                if (tileId >= 2 && tileId <= 9)
                {
                    // paths, z = 0
                    tile->setZValue(0.0);
                }
                else if (tileId >= 10 && tileId <= 15)
                {
                    // wall top & bottom z = 0.1
                    tile->setZValue((i < 10) ? 0.1 : 2.1);
                }
                else if (tileId >= 16 && tileId <= 19)
                {
                    // wall sides, z = 0.1
                    tile->setZValue(0.1);
                }
                else if (tileId >= 20 && tileId <= 29)
                {
                    // wall corners, z = 2.1
                    tile->setZValue(2.1);
                }

                addItem(tile);
            }
        }
    }
}
void Map::PlaceEntity(int startRow, int startCol, const QPixmap &image, qreal zValue)
{
    int widthInTiles = std::ceil((double)image.width() / TILE_SIZE);
    int heightInTiles = std::ceil((double)image.height() / TILE_SIZE);

    QGraphicsPixmapItem *entity = new QGraphicsPixmapItem(image);
    entity->setPos(startCol * TILE_SIZE, startRow * TILE_SIZE);
    entity->setZValue(zValue);
    addItem(entity);

    for (int i = startRow; i < startRow + heightInTiles; i++)
    {
        for (int j = startCol; j < startCol + widthInTiles; j++)
        {
            if (i < MAP_ROWS && j < MAP_COLS)
            {
                map[i][j] = 1; // Mark as occupied
            }
        }
    }
}
void Map::DrawField()
{
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            if (map[i][j] != 0)
                continue; // Skip if not grass

            int chance = QRandomGenerator::global()->bounded(200);
            QGraphicsPixmapItem *deco = nullptr;

            if (chance < 5)
                deco = new QGraphicsPixmapItem(Grass1);
            else if (chance < 8)
                deco = new QGraphicsPixmapItem(Grass3);
            else if (chance < 11)
                deco = new QGraphicsPixmapItem(Grass4);
            else if (chance < 14)
                deco = new QGraphicsPixmapItem(Grass5);
            else if (chance < 17)
                deco = new QGraphicsPixmapItem(Grass6);
            else if (chance < 18)
                deco = new QGraphicsPixmapItem(Stone1);
            else if (chance < 20)
                deco = new QGraphicsPixmapItem(Stone2);

            if (deco)
            {
                deco->setPos(j * TILE_SIZE, i * TILE_SIZE);
                deco->setZValue(0.1);
                addItem(deco);
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
    tileTextures[24].load(":resources/map-assets/wallbl");
    tileTextures[25].load(":resources/map-assets/wallblb");
    tileTextures[26].load(":resources/map-assets/wallbr");
    tileTextures[27].load(":resources/map-assets/wallbrb");

    // Gates
    tileTextures[28].load(":resources/map-assets/sidegate1");
    tileTextures[29].load(":resources/map-assets/sidegate4");

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

    // Specific Gates
    topgatel.load(":resources/map-assets/topgatel.png");
    topgater.load(":resources/map-assets/topgater.png");
    bottomgatel.load(":resources/map-assets/bottomgatel.png");
    bottomgater.load(":resources/map-assets/bottomgater.png");
    sidegate2.load(":resources/map-assets/sidegate2");
    sidegate3.load(":resources/map-assets/sidegate3");
}
