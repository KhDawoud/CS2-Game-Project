#ifndef MAP_H
#define MAP_H
#include <QGraphicsScene>
#include <QPixmap>
#include <unordered_map>
#include <vector>

constexpr int MAP_ROWS = 35;
constexpr int MAP_COLS = 40;
constexpr int TILE_SIZE = 32;

class Map : public QGraphicsScene
{
    std::vector<std::vector<int>> map;
    std::unordered_map<int, QPixmap> tileTextures;
    QPixmap House1;
    QPixmap House2;
    QPixmap House3;
    QPixmap House4;
    QPixmap Tent1;
    QPixmap Tent2;
    QPixmap Tent3;
    QPixmap Tent4;

    QPixmap Grass1;
    QPixmap Grass2;
    QPixmap Grass3;
    QPixmap Grass4;
    QPixmap Grass5;
    QPixmap Grass6;
    QPixmap Stone1;
    QPixmap Stone2;
    QPixmap Tree1;
    QPixmap Tree2;
    QPixmap Tree3;
    QPixmap Tree4;
    QPixmap Log1;
    QPixmap Log2;

    QPixmap CutDownLogs;
    QPixmap Cart;
    QPixmap Axe;
    QPixmap Lamp;
    QPixmap Barrel;
    QPixmap WaterWell;

    QPixmap topgatel;
    QPixmap topgater;
    QPixmap bottomgatel;
    QPixmap bottomgater;
    QPixmap sidegate2;
    QPixmap sidegate3;

public:
    Map();
    void LoadMapFromCSV(const QString &filePath);
    void DrawMap();
    void PlaceEntity(float, float, const QPixmap &, qreal, bool border);
    void DrawField();
    void ImageLoader();
    std::vector<std::vector<int>> collision_map;
    void DrawCollisionMap();
    void AddTrees();
};

#endif // MAP_H
