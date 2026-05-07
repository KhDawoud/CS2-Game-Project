#include "maploader.hpp"
#include "slime.hpp"
#include "campfire.hpp"
#include "AudioManager.hpp"
#include "player.hpp"

#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QDebug>
#include <cmath>

// ── Initialize Static Members ─────────────────────────────────────────────────
QHash<int, QPixmap> MapLoader::baseTileRegistry;
QHash<QString, CollidableTemplate> MapLoader::templateRegistry;
std::vector<QPixmap> MapLoader::decoPool;
bool MapLoader::assetsLoaded = false;

// ─────────────────────────────────────────────────────────────────────────────
MapLoader::MapLoader(const QString &jsonPath, Player *player, QObject *parent)
    : QGraphicsScene(parent), player(player)
{
    loadAssets();
    loadFromJson(jsonPath);
}

// ── Top-level JSON driver ─────────────────────────────────────────────────────
void MapLoader::loadFromJson(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "MapLoader: cannot open" << path;
        return;
    }
    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
    file.close();

    // 1. Meta (Map dimensions and collision rules)
    QJsonObject meta = root["meta"].toObject();
    MAP_ROWS = meta["rows"].toInt(MAP_ROWS);
    MAP_COLS = meta["cols"].toInt(MAP_COLS);
    TILE_SIZE = meta["tileSize"].toInt(TILE_SIZE);

    // Parse solid tiles specific to this map (Solves the "0 is grass vs void" bug)
    QJsonArray solidArr = meta["solidTiles"].toArray();
    for (const QJsonValue &val : solidArr)
    {
        solidTileIDs.insert(val.toInt());
    }

    // 2. Base tile layer
    QJsonArray rows = root["tileLayer"].toObject()["data"].toArray();

    mapData.clear();
    collisionMap.assign(MAP_ROWS, std::vector<int>(MAP_COLS, 0));

    // get map vector from json
    for (int r = 0; r < rows.size(); ++r)
    {
        QJsonArray row = rows[r].toArray();
        std::vector<int> rowVec;

        for (int c = 0; c < row.size(); ++c)
        {
            int id = row[c].toInt();
            rowVec.push_back(id);

            if (solidTileIDs.contains(id))
            {
                collisionMap[r][c] = 1;
            }
        }

        mapData.push_back(rowVec);
    }
    drawBaseTiles();
    drawFieldDecorations();

    // 3. Hand-authored static objects
    placeStaticObjects(root["staticObjects"].toArray());

    // 4. Procedural passes
    if (root.contains("randomCollidables") && !root["randomCollidables"].isNull())
        distributeRandomCollidables(root["randomCollidables"].toObject());

    if (root.contains("enemySpawns") && !root["enemySpawns"].isNull())
        spawnEnemies(root["enemySpawns"].toObject());
}

// ── Asset loading ─────────────────────────────────────────────────────────────
void MapLoader::loadAssets()
{
    if (assetsLoaded)
        return; // Prevent memory leak on map switch

    // Base tiles
    baseTileRegistry[0] = QPixmap(":resources/map-assets/grass.png");
    baseTileRegistry[2] = QPixmap(":resources/map-assets/up.png");
    baseTileRegistry[3] = QPixmap(":resources/map-assets/down.png");
    baseTileRegistry[4] = QPixmap(":resources/map-assets/left.png");
    baseTileRegistry[5] = QPixmap(":resources/map-assets/right.png");
    baseTileRegistry[6] = QPixmap(":resources/map-assets/center.png");
    baseTileRegistry[7] = QPixmap(":resources/map-assets/up-right.png");
    baseTileRegistry[8] = QPixmap(":resources/map-assets/right-up.png");
    baseTileRegistry[9] = QPixmap(":resources/map-assets/left-up.png");
    baseTileRegistry[37] = QPixmap(":resources/map-assets/right-down.png");

    // Non-collidable decoration pool
    decoPool = {
        QPixmap(":resources/map-assets/grass1.png"),
        QPixmap(":resources/map-assets/grass2.png"),
        QPixmap(":resources/map-assets/grass3.png"),
        QPixmap(":resources/map-assets/stone1.png"),
        QPixmap(":resources/map-assets/stone2.png")};

    auto reg = [&](const QString &id, const QString &path, QRectF hitbox)
    {
        templateRegistry[id] = {id, QPixmap(path), hitbox};
    };

    // --- OVERWORLD ASSETS ---
    reg("house1", ":resources/map-assets/house1.png", {10, 77, 137, 70});
    reg("house2", ":resources/map-assets/house2.png", {12, 45, 99, 65});
    reg("house3", ":resources/map-assets/house3.png", {10, 70, 140, 60});
    reg("tent1", ":resources/map-assets/tent1.png", {3, 26, 63, 39});
    reg("tent4", ":resources/map-assets/tent4.png", {0, 32, 60, 32});
    reg("tree1", ":resources/map-assets/TreeBasic1.png", {57, 77, 16, 24});
    reg("log1", ":resources/map-assets/LogBasic1.png", {15, 17, 19, 17});
    reg("cart", ":resources/map-assets/cart.png", {4, 7, 40, 20});
    reg("camplog1", ":resources/map-assets/camplog1.png", {6, 6, 16, 17});

    // Gates
    reg("topgatel", ":resources/map-assets/topgatel.png", {0, 0, 32, 8});
    reg("topgater", ":resources/map-assets/topgater.png", {0, 0, 30, 7});

    // --- INTERIOR ASSETS ---
    reg("bed", ":resources/house_interior/bed.png", {5, 10, 20, 40});
    reg("nightstand", ":resources/house_interior/nightstand.png", {2, 5, 20, 20});
    reg("dining_table", ":resources/house_interior/dining_table.png", {10, 20, 70, 40});
    reg("bookcase", ":resources/house_interior/bookcase.png", {0, 20, 30, 12});
    reg("stairs", ":resources/house_interior/stairs.png", {0, 0, 0, 0}); // 0 hitbox
    reg("window", ":resources/house_interior/window.png", {0, 0, 0, 0}); // 0 hitbox

    // Carpets (Non-collidable)
    reg("purple_carpet", ":resources/house_interior/purple_carpet.png", {0, 0, 0, 0});
    reg("red_carpet_rect", ":resources/house_interior/red_carpet_rect.png", {0, 0, 0, 0});

    assetsLoaded = true;
}


void MapLoader::drawBaseTiles()
{
    const QPixmap &grassPx = baseTileRegistry[0]; // Assuming 0 is grass background
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            // Background
            auto *base = new QGraphicsPixmapItem(grassPx);
            base->setPos(j * TILE_SIZE, i * TILE_SIZE);
            base->setZValue(-100.0);
            addItem(base);

            // Overlay Tile
            int id = mapData[i][j];
            if (id != 0 && baseTileRegistry.contains(id))
            {
                auto *tile = new QGraphicsPixmapItem(baseTileRegistry[id]);
                tile->setPos(j * TILE_SIZE, i * TILE_SIZE);
                tile->setZValue(-100.0);
                addItem(tile);
            }
        }
    }
}

void MapLoader::drawFieldDecorations()
{
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            if (mapData[i][j] != 0 && mapData[i][j] != 99)
                continue;

            if (QRandomGenerator::global()->bounded(100) < 30)
            {
                int idx = QRandomGenerator::global()->bounded((int)decoPool.size());
                auto *deco = new QGraphicsPixmapItem(decoPool[idx]);
                deco->setPos(j * TILE_SIZE, i * TILE_SIZE);
                deco->setZValue(-50.0);
                addItem(deco);
            }
        }
    }
}

// ── Static object dispatch ────────────────────────────────────────────────────
void MapLoader::placeStaticObjects(const QJsonArray &objects)
{
    for (const QJsonValue &val : objects)
    {
        QJsonObject obj = val.toObject();
        QString type = obj["type"].toString();
        float row = (float)obj["row"].toDouble();
        float col = (float)obj["col"].toDouble();

        if (type == "collidable")
        {
            placeCollidable(row, col, obj["templateId"].toString());
        }
        else if (type == "nonCollidable")
        {
            placeNonCollidable(row, col, obj["assetId"].toString(), obj["zValue"].toDouble(1000.0));
        }
        else if (type == "campfire")
        {
            addCampfire(row, col);
        }
    }
}

// ── Placement helpers ─────────────────────────────────────────────────────────
void MapLoader::placeCollidable(float row, float col, const QString &templateId)
{
    if (!templateRegistry.contains(templateId))
    {
        qWarning() << "MapLoader: unknown template id:" << templateId;
        return;
    }
    const CollidableTemplate &tmpl = templateRegistry[templateId];
    QPointF worldPos(col * TILE_SIZE, row * TILE_SIZE);

    auto *item = new QGraphicsPixmapItem(tmpl.texture);
    item->setPos(worldPos);

    QRectF worldHitbox = tmpl.hitbox.translated(worldPos);
    item->setZValue(worldHitbox.bottom());
    addItem(item);

    // Only add to activeCollidables if it actually has a hitbox
    if (tmpl.hitbox.width() > 0 && tmpl.hitbox.height() > 0)
    {
        activeCollidables.push_back({worldHitbox});
    }
}

void MapLoader::placeNonCollidable(float row, float col, const QString &assetId, qreal zValue)
{
    QPixmap px;
    if (templateRegistry.contains(assetId))
        px = templateRegistry[assetId].texture;
    else
        return;

    auto *item = new QGraphicsPixmapItem(px);
    item->setPos(col * TILE_SIZE, row * TILE_SIZE);
    item->setZValue(zValue);
    addItem(item);

    // Mask underlying grid so procedural items don't overlap
    int wTiles = std::ceil((double)px.width() / TILE_SIZE);
    int hTiles = std::ceil((double)px.height() / TILE_SIZE);
    for (int i = (int)row; i < (int)row + hTiles; i++)
        for (int j = (int)col; j < (int)col + wTiles; j++)
            if (i >= 0 && i < MAP_ROWS && j >= 0 && j < MAP_COLS)
                mapData[i][j] = 99;
}

void MapLoader::addCampfire(float row, float col)
{
    auto *cf = new Campfire();
    QPointF worldPos(col * TILE_SIZE, row * TILE_SIZE);
    cf->setPos(worldPos);
    cf->setZValue(worldPos.y() + cf->getheight());
    addItem(cf);
}

void MapLoader::spawnPlayer(Player *p, float row, float col)
{
    this->player = p;
    // Remove from old scene if necessary
    if (player->scene())
    {
        player->scene()->removeItem(player);
    }
    addItem(player);
    player->setPos(col * TILE_SIZE, row * TILE_SIZE);
    player->setZValue(player->y() + player->pixmap().height());
}

// ── Random collidable distribution ───────────────────────────────────────────
void MapLoader::distributeRandomCollidables(const QJsonObject &cfg)
{
    QJsonArray idArray = cfg["templateIds"].toArray();
    QStringList candidateIds;
    for (const QJsonValue &v : idArray)
        candidateIds << v.toString();
    if (candidateIds.isEmpty())
        return;

    int count = cfg["count"].toInt(40);
    int spacing = cfg["spacing"].toInt(1);
    int maxAttempts = count * 30;
    int placed = 0, attempts = 0;

    while (placed < count && attempts < maxAttempts)
    {
        attempts++;
        int randomRow = QRandomGenerator::global()->bounded(MAP_ROWS);
        int randomCol = QRandomGenerator::global()->bounded(MAP_COLS);

        QString id = candidateIds[QRandomGenerator::global()->bounded(candidateIds.size())];
        if (!templateRegistry.contains(id))
            continue;

        const CollidableTemplate &tmpl = templateRegistry[id];
        int wTiles = std::ceil((double)tmpl.texture.width() / TILE_SIZE);
        int hTiles = std::ceil((double)tmpl.texture.height() / TILE_SIZE);

        if (randomRow + hTiles > MAP_ROWS || randomCol + wTiles > MAP_COLS)
            continue;

        bool canPlace = true;
        int r0 = std::max(0, randomRow - spacing);
        int r1 = std::min(MAP_ROWS, randomRow + hTiles + spacing);
        int c0 = std::max(0, randomCol - spacing);
        int c1 = std::min(MAP_COLS, randomCol + wTiles + spacing);

        for (int r = r0; r < r1 && canPlace; r++)
            for (int c = c0; c < c1 && canPlace; c++)
                if (mapData[r][c] != 0 && mapData[r][c] != 99)
                    canPlace = false;

        if (!canPlace)
            continue;

        QRectF spacingRect(
            (randomCol - spacing) * TILE_SIZE,
            (randomRow - spacing) * TILE_SIZE,
            (wTiles + 2 * spacing) * TILE_SIZE,
            (hTiles + 2 * spacing) * TILE_SIZE);

        for (QGraphicsItem *item : items(spacingRect))
            if (item->zValue() > -50.0)
            {
                canPlace = false;
                break;
            }

        if (canPlace)
        {
            placeCollidable(randomRow, randomCol, id);
            placed++;
        }
    }
}

// ── Enemy spawning ────────────────────────────────────────────────────────────
void MapLoader::spawnEnemies(const QJsonObject &cfg)
{
    int count = cfg["count"].toInt(15);
    int rowMin = cfg["rowMin"].toInt(5);
    int rowMax = cfg["rowMax"].toInt(28);
    int colMin = cfg["colMin"].toInt(5);
    int colMax = cfg["colMax"].toInt(33);

    QJsonArray typeArray = cfg["types"].toArray();
    std::vector<EnemyType> types;
    int totalWeight = 0;

    for (const QJsonValue &v : typeArray)
    {
        QJsonObject t = v.toObject();
        int w = t["weight"].toInt(10);
        types.push_back({t["id"].toString(), t["variant"].toInt(1), w});
        totalWeight += w;
    }
    if (types.empty() || totalWeight == 0)
        return;

    int placed = 0, attempts = 0, maxAttempts = count * 100;
    while (placed < count && attempts < maxAttempts)
    {
        attempts++;
        int r = QRandomGenerator::global()->bounded(rowMin, rowMax);
        int c = QRandomGenerator::global()->bounded(colMin, colMax);

        if (mapData[r][c] != 0 && mapData[r][c] != 99)
            continue;

        QRectF spawnRect(c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE / 3, TILE_SIZE / 3);
        bool blocked = false;

        for (const auto &obj : activeCollidables)
        {
            if (spawnRect.intersects(obj.worldHitbox))
            {
                blocked = true;
                break;
            }
        }
        if (blocked)
            continue;

        int roll = QRandomGenerator::global()->bounded(totalWeight);
        int cumulative = 0;
        const EnemyType *chosen = &types.back();
        for (const auto &t : types)
        {
            cumulative += t.weight;
            if (roll < cumulative)
            {
                chosen = &t;
                break;
            }
        }

        Slime *slime = new Slime(chosen->variant);
        slime->setPos(c * TILE_SIZE, r * TILE_SIZE);
        slime->setPlayer(player);
        addItem(slime);
        connect(slime, &Slime::enemyDied, this, &MapLoader::onEnemyDied);

        mapData[r][c] = 98;
        placed++;
    }
    currentEnemyCount = placed;
}

// ── Signals & queries ─────────────────────────────────────────────────────────
void MapLoader::onEnemyDied()
{
    currentEnemyCount--;
    emit requestBarUpdate(currentEnemyCount);
    if (currentEnemyCount <= 0)
    {
        AudioManager::instance().playSound("LevelCleared");
        emit levelCleared();
    }
}

bool MapLoader::isTileCollidable(int row, int col) const
{
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
        return true;
    return collisionMap[row][col] == 1;
}