#ifndef MAP_H
#define MAP_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QRectF>
#include <vector>
#include <unordered_map>
#include "campfire.hpp"
#include "characterstats.hpp"


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
    Q_OBJECT
private:
    std::vector<std::vector<int>> mapData;
    std::vector<std::vector<int>> collision_map;
    std::unordered_map<int, QPixmap> baseTiles;
    std::vector<QPixmap> nonCollidableDecoPool;
    std::vector<CollidableTemplate> collidableTemplates;
    std::vector<PlacedObject> activeCollidableObjects;
    std::vector<Campfire *> activeCampfires;
    Player* player;
    CharacterStats *stats;
    QPixmap Tent4;
    QPixmap topgatel;
    QPixmap topgater;
    QPixmap bottomgatel;
    QPixmap bottomgater;
    QPixmap sidegate2;
    QPixmap sidegate3;
    int currentEnemyCount;

public:
    Map();

    void DrawDebugGridCoordinates();
    void LoadMapFromCSV(const QString &filePath);
    void ImageLoader();
    void DrawMapAndGenerateBaseCollisions();
    void PlaceMapStandardTiles();

    void DrawFieldDecorations();
    void DistributeRandomCollidables(int count, int minIndex, int maxIndex, int spacing);
    void NonCollidablePlaceEntity(float, float,const QPixmap&,qreal);
    void PlaceCollidable(float startRow, float startCol, int templateIndex);
    void AddCampfire(float startRow, float startCol);
    void AddEnemysRandomly(int count,int spacing);
    void AddPlayerandStats();
    Player* getPlayer();
    CharacterStats* getStats();
    bool isTileCollidable(int row, int col) const;
    const std::vector<PlacedObject> &getCollidableObjects() const { return activeCollidableObjects; }
    int getCurrentEnimies();
    //void keyPressEvent(QKeyEvent *event) override;
public slots:
    void updateEnemyCount();
signals:
    void requestBarUpdate(int currentCount);
};

#endif // MAP_H
