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

// ── Unified asset type ────────────────────────────────────────────────────────
struct CollidableTemplate
{
    QString id;
    QPixmap texture;
    QRectF hitbox; // local-space, relative to top-left of texture
};

// ── Runtime collision record (world-space) ───────────────────────────────────
struct ActiveCollidable
{
    QRectF worldHitbox;
};

// ── Enemy weight entry ────────────────────────────────────────────────────────
struct EnemyType
{
    QString id;
    int variant;
    int weight; // relative probability
};

class MapLoader : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit MapLoader(const QString &jsonPath, Player *player, QObject *parent = nullptr);

    // Collision Checkers
    bool isTileCollidable(int row, int col) const;
    const std::vector<ActiveCollidable> &getActiveCollidables() const { return activeCollidables; }

    // Spawn / Scene Management
    void spawnPlayer(Player *p, float row, float col);

    // Getters for external systems
    int tileSize() const { return TILE_SIZE; }
    int mapRows() const { return MAP_ROWS; }
    int mapCols() const { return MAP_COLS; }

signals:
    void requestBarUpdate(int remaining);
    void levelCleared();

private slots:
    void onEnemyDied();

private:
    // ── Map config (populated from JSON meta block) ───────────────────────────
    int MAP_ROWS = 35;
    int MAP_COLS = 40;
    int TILE_SIZE = 32;
    QSet<int> solidTileIDs; // Tiles that act as full-block walls (e.g., water, void)

    Player *player = nullptr;
    int currentEnemyCount = 0;

    // ── Static Asset Registries (Memory Optimization) ─────────────────────────
    // Made static so they are only loaded into RAM ONCE per game session
    static QHash<int, QPixmap> baseTileRegistry;                // CSV tile-id → pixmap
    static QHash<QString, CollidableTemplate> templateRegistry; // string id  → template
    static std::vector<QPixmap> decoPool;                       // non-collidable field art
    static bool assetsLoaded;

    // ── Runtime state ─────────────────────────────────────────────────────────
    std::vector<std::vector<int>> mapData;
    std::vector<std::vector<int>> collisionMap;
    std::vector<ActiveCollidable> activeCollidables;

    // ── Loading pipeline ──────────────────────────────────────────────────────
    void loadFromJson(const QString &path);
    void loadAssets();
    void drawBaseTiles();
    void drawFieldDecorations();
    void placeStaticObjects(const QJsonArray &objects);
    void distributeRandomCollidables(const QJsonObject &cfg);
    void spawnEnemies(const QJsonObject &cfg);

    // ── Placement helpers ─────────────────────────────────────────────────────
    void placeCollidable(float row, float col, const QString &templateId);
    void placeNonCollidable(float row, float col, const QString &assetId, qreal zValue);
    void addCampfire(float row, float col);
};