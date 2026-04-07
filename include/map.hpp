#ifndef MAP_H
#define MAP_H
#include <QGraphicsScene>
#include <QPixmap>
#include <unordered_map>
#include <vector>
#include <QTimer>
#include <QRectF>
#include "campfire.hpp"
constexpr int MAP_ROWS = 36;
constexpr int MAP_COLS = 41;
constexpr int TILE_SIZE = 32;

class Map : public QGraphicsScene
{
    std::vector<std::vector<int>> map;
    std::vector<std::vector<int>> collision_map;
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
    QPixmap Tree2;
    QPixmap Tree3;
    QPixmap Log2;
    QPixmap CampLog1;
    QPixmap CampLog2;
    Campfire *CampFire = new Campfire();
    Campfire *CampFire2 = new Campfire();

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
    void PlaceEntity(float, float,int, int, const QPixmap &, qreal);
    void PlaceEntityWithOffset(float, float,int, int, const QPixmap &, qreal,int=0);
    void NonCollidablePlaceEntity(float, float,const QPixmap&, qreal);
    void Addlamp(float, float,const QPixmap&);
    void DrawField();
    void ImageLoader();
    void DrawCollisionMap();
    void AddTrees();
    void AddCampfire(float, float, Campfire *);
    bool isTileCollidable(int, int);
};

#endif // MAP_H
