#ifndef MAP_H
#define MAP_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QRectF>
#include <vector>
#include <unordered_map>
#include "campfire.hpp"

constexpr int MAP_ROWS = 35;
constexpr int MAP_COLS = 40;
constexpr int TILE_SIZE = 32;

struct CollidableTemplate
{
    QPixmap texture;
    QRectF localHitbox;
};

struct PlacedObject
{
    QRectF worldHitbox;
};

class Map : public QGraphicsScene
{
    std::vector<std::vector<int>> mapData;
    std::vector<std::vector<int>> collision_map;

    std::unordered_map<int, QPixmap> baseTiles;
    std::vector<QPixmap> nonCollidableDecoPool;
    std::vector<CollidableTemplate> collidableTemplates;
    std::vector<PlacedObject> activeCollidableObjects;
    std::vector<Campfire *> activeCampfires;

public:
    Map();

    void DrawDebugGridCoordinates();
    void LoadMapFromCSV(const QString &filePath);
    void ImageLoader();
    void DrawMapAndGenerateBaseCollisions();
    void PlaceMapStandardTiles();

    void DrawFieldDecorations();
    void DistributeRandomCollidables(int count, int minIndex, int maxIndex, int spacing);

    void PlaceCollidable(float startRow, float startCol, int templateIndex);
    void AddCampfire(float startRow, float startCol);

    bool isTileCollidable(int row, int col) const;
    const std::vector<PlacedObject> &getCollidableObjects() const { return activeCollidableObjects; }
};

#endif // MAP_H
