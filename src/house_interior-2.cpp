#include "house_interior-2.hpp"
#include <QFile>
#include <QGraphicsPixmapItem>
#include <QRandomGenerator>
#include <algorithm>
#include <cmath>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────
House_Interior::House_Interior(Player *player1): player(player1)
{
    LoadMapFromCSV(":resources/house_interior/CSV/house_interior.csv");
    ImageLoader();   // fills collidableTemplates + nonCollidableDecoPool
    DrawMap();       // draws CSV base tiles
    addItem(player);
    player->setPos(16*11, 10*4);
    // ── NON-COLLIDABLE: Carpets & entrance mat ────────────────────────────────
    // These are purely visual – they use NonCollidablePlaceEntity so they never
    // block movement and are not added to activeCollidableObjects.

    NonCollidablePlaceEntity(5.5, 6, nonCollidableDecoPool[POOL_PURPLE_CARPET], 0.5);
    NonCollidablePlaceEntity(8, 11, nonCollidableDecoPool[POOL_RED_CARPET_OVAL_2], 0.5);

    NonCollidablePlaceEntity(2, 9.9, nonCollidableDecoPool[POOL_RED_CARPET_RECT_2], 0.5);
    NonCollidablePlaceEntity(2, 9, nonCollidableDecoPool[POOL_RED_CARPET_RECT_2], 0.5);
    NonCollidablePlaceEntity(2, 8, nonCollidableDecoPool[POOL_RED_CARPET_RECT_2], 0.5);
    NonCollidablePlaceEntity(2, 7, nonCollidableDecoPool[POOL_RED_CARPET_RECT_2], 0.5);
    NonCollidablePlaceEntity(2, 6, nonCollidableDecoPool[POOL_RED_CARPET_RECT_2], 0.5);
    NonCollidablePlaceEntity(2, 5.1, nonCollidableDecoPool[POOL_RED_CARPET_RECT_2], 0.5);

    NonCollidablePlaceEntity(3, 9.9, nonCollidableDecoPool[POOL_RED_CARPET_RECT], 0.5);
    NonCollidablePlaceEntity(3, 9, nonCollidableDecoPool[POOL_RED_CARPET_RECT], 0.5);
    NonCollidablePlaceEntity(3, 8, nonCollidableDecoPool[POOL_RED_CARPET_RECT], 0.5);
    NonCollidablePlaceEntity(3, 7, nonCollidableDecoPool[POOL_RED_CARPET_RECT], 0.5);
    NonCollidablePlaceEntity(3, 6, nonCollidableDecoPool[POOL_RED_CARPET_RECT], 0.5);
    NonCollidablePlaceEntity(3, 5.1, nonCollidableDecoPool[POOL_RED_CARPET_RECT], 0.5);

    NonCollidablePlaceEntity(8.85, 11.8, nonCollidableDecoPool[POOL_ENTRANCE], 1.0);

    // z = bottom edge of the wall tile in world space
    // row 6, and the wall sprite is 32px tall, so bottom = (6 * 32) + 32 = 224
    NonCollidablePlaceEntity(6, 2, nonCollidableDecoPool[POOL_WALL_NO_DOOR], (6 * SIZE) + SIZE*1.5);
    NonCollidablePlaceEntity(5, 13, nonCollidableDecoPool[POOL_WALL_NO_DOOR], (5 * SIZE) + SIZE*1.5);
    NonCollidablePlaceEntity(1,   12,   nonCollidableDecoPool[POOL_BANANA], 150);
    NonCollidablePlaceEntity(1.5, 12,   nonCollidableDecoPool[POOL_BANANA], 150);
    NonCollidablePlaceEntity(3,   12,   nonCollidableDecoPool[POOL_GARLIC], 150);
    NonCollidablePlaceEntity(3,   12.3, nonCollidableDecoPool[POOL_COAL],   150);

    // ── COLLIDABLE: Furniture, walls, objects ─────────────────────────────────
    // These call PlaceCollidable which auto-calculates z from hitbox.bottom()
    // and stores the world hitbox in activeCollidableObjects.

    // --- DINING AREA ---
    PlaceCollidable(6,  6.65, IDX_DINING_TABLE);

    // --- BEDROOM AREA ---
    PlaceCollidable(2, 5.6, IDX_BED);
    PlaceCollidable(2, 5, IDX_NIGHTSTAND);
    PlaceCollidable(1, 10, IDX_BOOKCASE);
    PlaceCollidable(1, 9, IDX_BOOKCASE);
    PlaceCollidable(2, 10, IDX_COUCH_SIDE);
    PlaceCollidable(1.5, 8, IDX_WINDOW);
    PlaceCollidable(1.5, 7, IDX_WINDOW);

    // --- PANTRY / STORAGE ---
    PlaceCollidable(1, 13, IDX_CUPBOARD_FULL);
    PlaceCollidable(2.5, 14, IDX_WOODEN_CRATES);
    PlaceCollidable(1, 11, IDX_CUPBOARD_EMPTY);
    //PlaceCollidable(1, 12, IDX_BANANA);
    //PlaceCollidable(1.5, 11.5, IDX_BANANA);
    PlaceCollidable(3, 12, IDX_ROUND_TABLE);
    //PlaceCollidable(3, 12.3, IDX_COAL);
    //PlaceCollidable(3, 12.6, IDX_GARLIC);

    // --- ARMORY ---
    PlaceCollidable(6, 11, IDX_SWORD_HOLDER);

    // --- ALCHEMY / ENTRANCE ---
    PlaceCollidable(4, 2, IDX_STAIRS);
    PlaceCollidable(7.5, 1, IDX_POTION_TABLE);
    PlaceCollidable(8, 2, IDX_WOODEN_BARREL);

    // --- WALLS ---
    PlaceCollidable(4, 5, IDX_WALL_RIGHT_TURN);
    //PlaceCollidable(6, 2, IDX_WALL_NO_DOOR);
    PlaceCollidable(6, 1, IDX_WALL_LEFT);
    PlaceCollidable(6, 3, IDX_WALL_RIGHT);
    PlaceCollidable(6, 3, IDX_SIDE_OF_WALL_2);

    //PlaceCollidable(5, 13, IDX_WALL_NO_DOOR);
    PlaceCollidable(5, 12, IDX_WALL_MIDDLE);
    PlaceCollidable(5, 11, IDX_WALL_MIDDLE);
    PlaceCollidable(5, 14, IDX_WALL_RIGHT);
    PlaceCollidable(5, 10, IDX_WALL_LEFT_TURN);
    PlaceCollidable(1.1, 10, IDX_SIDE_OF_WALL);

    DrawCollisionMap();
}

// ─────────────────────────────────────────────────────────────────────────────
// ImageLoader  –  fills collidableTemplates and nonCollidableDecoPool
//
// HITBOX FORMAT:  QRectF(x, y, width, height)
//   x, y   → offset from the sprite's top-left corner (in pixels)
//   width  → hitbox width  in pixels
//   height → hitbox height in pixels
//
// All values marked "PLACEHOLDER" need to be tuned to match your actual sprites.
// Tip: temporarily uncomment the debug-rect block in PlaceCollidable to see
//      the hitboxes drawn in red on screen.
// ─────────────────────────────────────────────────────────────────────────────
void House_Interior::ImageLoader()
{
    // ── Base tile textures (CSV tile IDs) ────────────────────────────────────
    tileTextures[0].load(":resources/house_interior/Using/black_image.png");

    tileTextures[2].load(":resources/house_interior/Using/floor_left.png");
    tileTextures[3].load(":resources/house_interior/Using/floor_right.png");
    tileTextures[4].load(":resources/house_interior/Using/floor_bottom.png");
    tileTextures[5].load(":resources/house_interior/Using/floor_bottom_left_corner.png");
    tileTextures[6].load(":resources/house_interior/Using/floor_bottom_right_corner.png");
    tileTextures[7].load(":resources/house_interior/Using/floor_top_left_corner.png");
    tileTextures[8].load(":resources/house_interior/Using/floor_top_right_corner.png");
    tileTextures[9].load(":resources/house_interior/Using/floor_middle.png");

    tileTextures[10].load(":resources/house_interior/Using/wall_middle.png");
    tileTextures[11].load(":resources/house_interior/Using/wall_left.png");
    tileTextures[12].load(":resources/house_interior/Using/wall_right.png");

    // ── NON-COLLIDABLE pool  (order must match POOL_* constants in .hpp) ─────
    // POOL_PURPLE_CARPET = 0
    nonCollidableDecoPool.push_back(
        QPixmap(":resources/house_interior/Using-Deco/purple_carpet_circle.png"));
    // POOL_RED_CARPET_OVAL_2 = 1
    nonCollidableDecoPool.push_back(
        QPixmap(":resources/house_interior/Using-Deco/red_carpet_oval_2.png"));
    // POOL_RED_CARPET_RECT = 2
    nonCollidableDecoPool.push_back(
        QPixmap(":resources/house_interior/Using-Deco/red_carpet_rectangle.png"));
    // POOL_RED_CARPET_RECT_2 = 3
    nonCollidableDecoPool.push_back(
        QPixmap(":resources/house_interior/Using-Deco/red_carpet_rectangle_2.png"));
    // POOL_ENTRANCE = 4
    nonCollidableDecoPool.push_back(
        QPixmap(":resources/house_interior/Using/entrance.png"));
    // POOL_WALL_NO_DOOR = 5;
    nonCollidableDecoPool.push_back(
        QPixmap(":resources/house_interior/Using/wall_no_door.png"));
    // POOL_BANANA = 6
    nonCollidableDecoPool.push_back(
        QPixmap(":resources/house_interior/Using-Deco/banana.png"));
    // POOL_GARLIC = 7
    nonCollidableDecoPool.push_back(
        QPixmap(":resources/house_interior/Using-Deco/garlic.png"));
    // POOL_COAL = 8
    nonCollidableDecoPool.push_back(
        QPixmap(":resources/house_interior/Using-Deco/coal.png"));


    // ── COLLIDABLE templates  (order must match IDX_* constants in .hpp) ─────
    // Each entry: { QPixmap, QRectF(x, y, w, h) }
    // Replace every "PLACEHOLDER" comment once you know the real sprite sizes.

    // IDX_BED = 0
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/bed.png"),
         QRectF(0, 0, 20, 32)}); // PLACEHOLDER – adjust y/h so only the base blocks

    // IDX_NIGHTSTAND = 1
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/nightstand.png"),
         QRectF(5, 8, 4, 12)}); // PLACEHOLDER

    // IDX_BOOKCASE = 2
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/bookcase.png"),
         QRectF(0, 16, 32, 22)}); // PLACEHOLDER

    // IDX_COUCH_SIDE = 3
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/couch_side_profile.png"),
         QRectF(10, 22, 24, 16)}); // PLACEHOLDER

    // IDX_DINING_TABLE = 4
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/dining_table.png"),
         QRectF(4, 20, 64, 32)}); // PLACEHOLDER

    // IDX_CUPBOARD_FULL = 5
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/cupboard_full.png"),
         QRectF(0, 16, 50, 32)}); // PLACEHOLDER

    // IDX_CUPBOARD_EMPTY = 6
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/cupboard_empty.png"),
         QRectF(0, 16, 42, 32)}); // PLACEHOLDER

    // IDX_WOODEN_CRATES = 7
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/wooden_crates.png"),
         QRectF(9, 10, 10, 10)}); // PLACEHOLDER

    // IDX_ROUND_TABLE = 8
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/round_table.png"),
         QRectF(4, 12, 34, 20)}); // PLACEHOLDER

    // IDX_SWORD_HOLDER = 9
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/sword_holder.png"),
         QRectF(0, 8, 60, 24)}); // PLACEHOLDER

    // IDX_POTION_TABLE = 10
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/potion_table.png"),
         QRectF(2, 16, 28, 13)}); // PLACEHOLDER

    // IDX_WOODEN_BARREL = 11
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/wooden_barrel.png"),
         QRectF(4, 8, 30, 117)}); // PLACEHOLDER

    // IDX_STAIRS = 12
    // Stairs are non-blocking for movement (player walks on them), so hitbox
    // is zeroed out. If you want them solid, give them a real rect.
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/stairs.png"),
         QRectF(0, 0, 0, 0)}); // intentionally no collision

    // IDX_WALL_NO_DOOR = 13
    //collidableTemplates.push_back(
    //    {QPixmap(":resources/house_interior/Using/wall_no_door.png"),
     //    QRectF(0, 0, 0, 0)}); // PLACEHOLDER – full tile

    // IDX_WALL_MIDDLE = 14
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using/wall_middle.png"),
         QRectF(0, 20, 36, 25)}); // PLACEHOLDER

    // IDX_WALL_LEFT = 15
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using/wall_left.png"),
         QRectF(0, 20, 36, 15)}); // PLACEHOLDER

    // IDX_WALL_RIGHT = 16
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using/wall_right.png"),
         QRectF(-4, 20, 36,15)}); // PLACEHOLDER

    // IDX_WALL_RIGHT_TURN = 17
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using/wall_right_turn.png"),
         QRectF(0, 0, 0.5, 32)}); // PLACEHOLDER

    // IDX_WALL_LEFT_TURN = 18
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using/wall_left_turn.png"),
         QRectF(30, 0, 2,32)}); // PLACEHOLDER

    // IDX_SIDE_OF_WALL = 19
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using/side_of_wall.png"),
         QRectF(32, 0, 1, 130)}); // PLACEHOLDER

    // IDX_SIDE_OF_WALL_2 = 20
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using/side_of_wall_2.png"),
         QRectF(32, 20, 1, 96)}); // PLACEHOLDER

    // IDX_WINDOW = 21
    // Windows are visual; zero hitbox keeps them non-blocking
    collidableTemplates.push_back(
        {QPixmap(":resources/house_interior/Using-Deco/window.png"),
         QRectF(0, 0, 0, 0)}); // intentionally no collision


}

// ─────────────────────────────────────────────────────────────────────────────
// LoadMapFromCSV  –  unchanged from original
// ─────────────────────────────────────────────────────────────────────────────
void House_Interior::LoadMapFromCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ERROR: Could not open the map file:" << filePath;
        return;
    }
    map.clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(',');
        std::vector<int> row;
        for (const QString &val : values)
            row.push_back(val.trimmed().toInt());
        map.push_back(row);
    }
    file.close();
}

// ─────────────────────────────────────────────────────────────────────────────
// DrawMap  –  unchanged from original (renders CSV base tiles)
// ─────────────────────────────────────────────────────────────────────────────
void House_Interior::DrawMap()
{
    for (int i = 0; i < HOUSE_ROWS; i++) {
        for (int j = 0; j < HOUSE_COLS; j++) {
            QGraphicsPixmapItem *base = new QGraphicsPixmapItem(tileTextures[0]);
            base->setPos(j * SIZE, i * SIZE);
            addItem(base);

            int tileId = map[i][j];
            if (tileId != 0 && tileTextures.find(tileId) != tileTextures.end()) {
                QGraphicsPixmapItem *tile = new QGraphicsPixmapItem(tileTextures[tileId]);
                tile->setPos(j * SIZE, i * SIZE);

                if (tileId >= 2 && tileId <= 9)
                    tile->setZValue(0);       // floor tiles
                else if (tileId >= 10 && tileId <= 12)
                    tile->setZValue(1);       // base-layer walls

                addItem(tile);
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// PlaceCollidable  –  mirrors Map::PlaceCollidable exactly
//   • Spawns the sprite at (startCol*SIZE, startRow*SIZE)
//   • Translates localHitbox to world space
//   • Sets z = hitbox.bottom() so sprites behind the player render correctly
//   • Appends the world hitbox to activeCollidableObjects
// ─────────────────────────────────────────────────────────────────────────────
void House_Interior::PlaceCollidable(float startRow, float startCol, int templateIndex)
{
    if (templateIndex < 0 || templateIndex >= static_cast<int>(collidableTemplates.size()))
        return;

    const HouseCollidableTemplate &tmpl = collidableTemplates[templateIndex];
    QPointF worldPos(startCol * SIZE, startRow * SIZE);

    QGraphicsPixmapItem *entity = new QGraphicsPixmapItem(tmpl.texture);
    entity->setPos(worldPos);

    QRectF worldHitbox = tmpl.localHitbox.translated(worldPos);
    entity->setZValue(worldHitbox.bottom());
    addItem(entity);

    // Only register a real hitbox (skip zero-area entries like stairs/windows)
    if (!tmpl.localHitbox.isEmpty())
        activeCollidableObjects.push_back({worldHitbox});

//    ── Debug: uncomment to see hitboxes drawn in red ────────────────────────
//     QGraphicsRectItem *debugBox = new QGraphicsRectItem(worldHitbox);
//     debugBox->setBrush(QBrush(QColor(255, 0, 0, 100)));
//     debugBox->setPen(QPen(Qt::red));
//     debugBox->setZValue(10000);
//     addItem(debugBox);
 }

// ─────────────────────────────────────────────────────────────────────────────
// NonCollidablePlaceEntity  –  mirrors Map::NonCollidablePlaceEntity
//   • Spawns the sprite visually at the given position
//   • Does NOT add anything to activeCollidableObjects
//   • Does NOT modify the map[] grid
// ─────────────────────────────────────────────────────────────────────────────
void House_Interior::NonCollidablePlaceEntity(float startRow, float startCol,
                                              const QPixmap &image, qreal zValue)
{
    QGraphicsPixmapItem *entity = new QGraphicsPixmapItem(image);
    entity->setPos(startCol * SIZE, startRow * SIZE);
    entity->setZValue(zValue);
    addItem(entity);


    // mark cells as occupied so nothing else places on top
    int widthInTiles  = std::ceil((double)image.width()  / SIZE);
    int heightInTiles = std::ceil((double)image.height() / SIZE);

    for (int i = qRound(startRow); i < startRow + heightInTiles; i++) {
        for (int j = qRound(startCol); j < startCol + widthInTiles; j++) {
            if (i >= 0 && i < HOUSE_ROWS && j >= 0 && j < HOUSE_COLS && map[i][j] != 0) {
                map[i][j] = 99;
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// isTileCollidable  –  checks the collision_map grid (for CSV-derived walls)
// ─────────────────────────────────────────────────────────────────────────────
bool House_Interior::isTileCollidable(int row, int col) const
{
    if (row < 0 || row >= HOUSE_ROWS || col < 0 || col >= HOUSE_COLS)
        return true; // out of bounds = solid
    return collision_map[row][col] == 1;
}

// ─────────────────────────────────────────────────────────────────────────────
// DrawCollisionMap  –  builds the tile-based collision grid from CSV tile IDs
//   This only covers wall tiles baked into the CSV.
//   Furniture collision is handled via activeCollidableObjects (world-space).
// ─────────────────────────────────────────────────────────────────────────────


void House_Interior::DrawCollisionMap()
{
    collision_map.assign(HOUSE_ROWS, std::vector<int>(HOUSE_COLS, 0));

    for (int i = 0; i < HOUSE_ROWS; i++) {
        for (int j = 0; j < HOUSE_COLS; j++) {
            int id = map[i][j];
            // block all border and wall tiles
            if (id == 0 || id == 10 || id == 11 || id == 12)
            {
                collision_map[i][j] = 1;
                activeCollidableObjects.push_back({
                    QRectF(j * SIZE, i * SIZE, SIZE, SIZE + 10)
                });

                /*QGraphicsRectItem *debugBox = new QGraphicsRectItem(
                    j * SIZE, i * SIZE, SIZE, SIZE);
                debugBox->setBrush(QBrush(QColor(255, 0, 0, 80)));
                debugBox->setPen(QPen(Qt::red));
                debugBox->setZValue(10000);
                addItem(debugBox);*/
            }
            else
            {
                collision_map[i][j] = 0;
            }
        }
    }
}
