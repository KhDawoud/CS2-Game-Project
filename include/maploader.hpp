#pragma once

#include <QGraphicsScene>
#include <QHash>
#include <QPixmap>
#include <QRectF>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <vector>

class Player;
class Campfire;

// all info for an object
struct CollidableTemplate
{
    QString id;
    QPixmap texture;
    QRectF hitbox;
};

// this stores the actual place in the scene the hitbox exists
struct ActiveCollidable
{
    QRectF worldHitbox;
};

struct EnemyType
{
    QString id;
    int variant;
    int weight; // rarity of it spawning
};

class MapLoader : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit MapLoader(const QString &jsonPath, Player *player, QObject *parent = nullptr);

    bool isTileCollidable(int row, int col) const;
    const std::vector<ActiveCollidable> &getActiveCollidables() const { return activeCollidables; }

    void spawnPlayer(Player *p, float row, float col);

    int tileSize() const { return TILE_SIZE; }
    int mapRows() const { return MAP_ROWS; }
    int mapCols() const { return MAP_COLS; }

    int getCurrentEnemyCount();

signals:
    void requestBarUpdate(int remaining);
    void levelCleared();

private slots:
    void onEnemyDied();

private:
    int MAP_ROWS = 35;
    int MAP_COLS = 40;
    int TILE_SIZE = 32;
    QSet<int> solidTileIDs;
    QSet<int> overheadTiles;

    Player *player = nullptr;
    int currentEnemyCount = 0;

    QHash<int, QPixmap> baseTileRegistry;
    std::vector<QPixmap> decoPool;
    int backgroundTileId = 0;
    bool hasFieldDecorations = false;

    // static just means they're only made once per file not everytime you make an instance
    static QHash<QString, CollidableTemplate> templateRegistry;
    static bool assetsLoaded;

    std::vector<std::vector<int>> mapData;
    std::vector<std::vector<int>> collisionMap;
    std::vector<ActiveCollidable> activeCollidables;

    void loadFromJson(const QString &path);
    void loadAssets();
    void drawBaseTiles();
    void drawFieldDecorations();
    void placeStaticObjects(const QJsonArray &objects);
    void distributeRandomCollidables(const QJsonObject &cfg);
    void spawnEnemies(const QJsonObject &cfg);

    void placeCollidable(float row, float col, const QString &templateId);
    void placeNonCollidable(float row, float col, const QString &assetId, qreal zValue);
    void addCampfire(float row, float col);
};