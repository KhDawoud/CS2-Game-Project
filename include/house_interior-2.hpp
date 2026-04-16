#ifndef HOUSE_INTERIOR_HPP
#define HOUSE_INTERIOR_HPP

#include <QGraphicsScene>
#include <QPixmap>
#include <QRectF>
#include <unordered_map>
#include <vector>
#include <QTimer>
#include "characterstats.hpp"

constexpr int HOUSE_ROWS = 10;
constexpr int HOUSE_COLS = 16;
constexpr int SIZE = 32;

// ── Mirrors the struct from Map ──────────────────────────────────────────────
struct HouseCollidableTemplate
{
    QPixmap texture;
    QRectF  localHitbox; // relative to the sprite's top-left corner
};

struct HousePlacedObject
{
    QRectF worldHitbox; // absolute position in scene coordinates
};
// ─────────────────────────────────────────────────────────────────────────────

class House_Interior : public QGraphicsScene
{
public:
    House_Interior(Player *player1);

    void LoadMapFromCSV(const QString &filePath);
    void ImageLoader();
    void DrawMap();
    void DrawCollisionMap();

    // Mirrors Map::PlaceCollidable  → places from collidableTemplates by index
    void PlaceCollidable(float startRow, float startCol, int templateIndex);

    // Mirrors Map::NonCollidablePlaceEntity → visual-only, marks no collision
    void NonCollidablePlaceEntity(float startRow, float startCol,
                                  const QPixmap &image, qreal zValue);

    // Kept for tile-based wall collision (CSV walls)
    bool isTileCollidable(int row, int col) const;

    const std::vector<HousePlacedObject> &getCollidableObjects() const
    { return activeCollidableObjects; }

private:
    Player *player;
    // ── Tile data ─────────────────────────────────────────────────────────────
    std::vector<std::vector<int>>    map;           // raw CSV data
    std::vector<std::vector<int>>    collision_map; // 1 = blocked, 0 = free
    std::unordered_map<int, QPixmap> tileTextures;  // base-layer tiles (CSV ids)

    // ── Object pools (mirrors Map's design) ───────────────────────────────────
    std::vector<HouseCollidableTemplate> collidableTemplates;
    std::vector<QPixmap>                 nonCollidableDecoPool;
    std::vector<HousePlacedObject>       activeCollidableObjects;

    // ── Template index constants ───────────────────────────────────────────────
    // Collidable furniture / walls  (used with PlaceCollidable)
    static constexpr int IDX_BED               = 0;
    static constexpr int IDX_NIGHTSTAND        = 1;
    static constexpr int IDX_BOOKCASE          = 2;
    static constexpr int IDX_COUCH_SIDE        = 3;
    static constexpr int IDX_DINING_TABLE      = 4;
    static constexpr int IDX_CUPBOARD_FULL     = 5;
    static constexpr int IDX_CUPBOARD_EMPTY    = 6;
    static constexpr int IDX_WOODEN_CRATES     = 7;
    static constexpr int IDX_ROUND_TABLE       = 8;
    static constexpr int IDX_SWORD_HOLDER      = 9;
    static constexpr int IDX_POTION_TABLE      = 10;
    static constexpr int IDX_WOODEN_BARREL     = 11;
    static constexpr int IDX_STAIRS            = 12;
    static constexpr int IDX_WALL_MIDDLE       = 13;
    static constexpr int IDX_WALL_LEFT         = 14;
    static constexpr int IDX_WALL_RIGHT        = 15;
    static constexpr int IDX_WALL_RIGHT_TURN   = 16;
    static constexpr int IDX_WALL_LEFT_TURN    = 17;
    static constexpr int IDX_SIDE_OF_WALL      = 18;
    static constexpr int IDX_SIDE_OF_WALL_2    = 19;
    static constexpr int IDX_WINDOW            = 20;


    // Non-collidable deco pool indices  (used with NonCollidablePlaceEntity)
    // Access via: nonCollidableDecoPool[POOL_*]
    static constexpr int POOL_PURPLE_CARPET         = 0;
    static constexpr int POOL_RED_CARPET_OVAL_2     = 1;
    static constexpr int POOL_RED_CARPET_RECT       = 2;
    static constexpr int POOL_RED_CARPET_RECT_2     = 3;
    static constexpr int POOL_ENTRANCE              = 4;
    static constexpr int POOL_WALL_NO_DOOR               = 5;
    static constexpr int POOL_BANANA  = 6;
    static constexpr int POOL_GARLIC  = 7;
    static constexpr int POOL_COAL    = 8;
};

#endif // HOUSE_INTERIOR_HPP
