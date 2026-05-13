#include "maploader.hpp"
#include "slime.hpp"
#include "boss.hpp"
#include "campfire.hpp"
#include "AudioManager.hpp"
#include "player.hpp"
#include "key.hpp"
#include "Vampires.hpp"

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

// this hashmap stores all our assets for all maps and we load them only once
QHash<QString, CollidableTemplate> MapLoader::templateRegistry;
bool MapLoader::assetsLoaded = false;

MapLoader::MapLoader(const QString &jsonPath, Player *player, QObject *parent)
    : QGraphicsScene(parent), player(player)
{
    loadAssets();
    loadFromJson(jsonPath);
}

// extract all our actual map data
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

    QJsonObject meta = root["meta"].toObject();
    MAP_ROWS = meta["rows"].toInt(MAP_ROWS);
    MAP_COLS = meta["cols"].toInt(MAP_COLS);
    TILE_SIZE = meta["tileSize"].toInt(TILE_SIZE);

    // solid tiles are our unwalkable tiles (zay el black space fel house)
    QJsonArray solidArr = meta["solidTiles"].toArray();
    for (const QJsonValue &val : solidArr)
    {
        solidTileIDs.insert(val.toInt());
    }

    QJsonArray overheadArr = meta["overheadTiles"].toArray();
    for (const QJsonValue &v : overheadArr)
    {
        overheadTiles.insert(v.toInt());
    }

    // get the textures for everything
    backgroundTileId = meta["backgroundTileId"].toInt(0);
    hasFieldDecorations = meta["hasFieldDecorations"].toBool(false);

    QJsonObject tilesObj = meta["tiles"].toObject();
    for (auto it = tilesObj.begin(); it != tilesObj.end(); ++it)
    {
        int id = it.key().toInt();
        baseTileRegistry[id] = QPixmap(it.value().toString());
    }

    QJsonArray decoArr = meta["decorationPool"].toArray();
    for (const QJsonValue &v : decoArr)
    {
        decoPool.push_back(QPixmap(v.toString()));
    }

    QJsonArray rows = root["tileLayer"].toObject()["data"].toArray();

    mapData.clear();

    // create the collison map we use in player
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

    // place our specifc collideables
    placeStaticObjects(root["staticObjects"].toArray());
    drawFieldDecorations();

    // place our random collidables
    if (root.contains("randomCollidables") && !root["randomCollidables"].isNull())
    {
        distributeRandomCollidables(root["randomCollidables"].toObject());
    }

    currentEnemyCount = 0;

    if (root.contains("enemySpawns") && !root["enemySpawns"].isNull())
    {
        spawnEnemies(root["enemySpawns"].toObject());
    }
    if (root.contains("specificEnemies") && !root["specificEnemies"].isNull())
    {
        spawnSpecificEnemies(root["specificEnemies"].toArray());
    }
    initialEnemyCount = currentEnemyCount;

    // spawn player when u load in specified spot (scene is still null)
    if (player && player->scene() == nullptr)
    {
        QJsonObject spawn = meta["playerSpawn"].toObject();

        if (!spawn.isEmpty())
        {
            float row = spawn["row"].toDouble();
            float col = spawn["col"].toDouble();

            spawnPlayer(player, row, col);
        }
    }
    QJsonObject key = meta["keySpawn"].toObject();

    if (!key.isEmpty())
    {
        float row = key["row"].toDouble();
        float col = key["col"].toDouble();
        Key *key1 = new Key(col * TILE_SIZE, row * TILE_SIZE, player, 1);
        addItem(key1);
    }
    Key *key2 = new Key(39 * TILE_SIZE, 22 * TILE_SIZE, player, 2);
    addItem(key2);
}

void MapLoader::loadAssets()
{
    if (assetsLoaded)
        return;

    auto reg = [&](const QString &id, const QString &path, QRectF hitbox)
    {
        templateRegistry[id] = {id, QPixmap(path), hitbox};
    };

    // would be good to organise these at some point
    reg("house1", ":resources/map-assets/house1.png", {10, 77, 137, 70});
    reg("house2", ":resources/map-assets/house2.png", {12, 45, 99, 65});
    reg("house3", ":resources/map-assets/house3.png", {10, 70, 140, 60});
    reg("house4", ":resources/map-assets/house4.png", {23, 70, 102, 88});

    reg("tent2", ":resources/map-assets/tent2.png", {10, 30, 52, 31});
    reg("tent3", ":resources/map-assets/tent3.png", {2, 30, 60, 33});
    reg("tent1", ":resources/map-assets/tent1.png", {3, 26, 63, 39});
    reg("tent4", ":resources/map-assets/tent4.png", {0, 32, 60, 32});
    reg("tree1", ":resources/map-assets/TreeBasic1.png", {57, 77, 16, 24});
    reg("log1", ":resources/map-assets/LogBasic1.png", {15, 17, 19, 17});
    reg("tree2", ":resources/map-assets/TreeBasic2.png", {26, 43, 13, 19});
    reg("tree3", ":resources/map-assets/TreeBasic3.png", {27, 38, 12, 12});
    reg("tree4", ":resources/map-assets/TreeBasic4.png", {58, 74, 11, 21});

    reg("log2", ":resources/map-assets/LogBasic2.png", {23, 26, 21, 10});
    reg("cart", ":resources/map-assets/cart.png", {4, 7, 40, 20});
    reg("camplog1", ":resources/map-assets/camplog1.png", {6, 6, 16, 17});
    reg("cutdownlogs", ":resources/map-assets/cutdownlogs.png", {6, 5, 10, 16});
    reg("axe", ":resources/map-assets/axe.png", {1, 10, 11, 8});
    reg("barrel", ":resources/map-assets/barrel.png", {0, 0, 20, 22});
    reg("camplog2", ":resources/map-assets/camplog2.png", {3, 4, 39, 8});
    reg("bottomgatel", ":resources/map-assets/bottomgatel.png", {0, 0, 1, 30});
    reg("bottomgater", ":resources/map-assets/bottomgater.png", {31, 5, 1, 30});

    reg("sidegate2", ":resources/map-assets/sidegate2.png", {16, 0, 9, 23});
    reg("sidegate3", ":resources/map-assets/sidegate3.png", {19, 0, 6, 32});
    reg("wallt", ":resources/map-assets/wallt.png", {0, 30, 32, 2});
    reg("wallt2", ":resources/map-assets/wallt2.png", {0, 30, 32, 2});
    reg("wallt3", ":resources/map-assets/wallt3.png", {0, 30, 32, 2});

    reg("walltb1", ":resources/map-assets/walltb1.png", {0, 0, 31, 26});
    reg("walltb2", ":resources/map-assets/walltb2.png", {0, 0, 32, 27});
    reg("walltb3", ":resources/map-assets/walltb3.png", {0, 0, 32, 28});

    reg("topgatel", ":resources/map-assets/topgatel.png", {0, 0, 32, 8});
    reg("topgater", ":resources/map-assets/topgater.png", {0, 0, 30, 7});

    reg("bed", ":resources/house_interior/Using-Deco/bed.png", {0, 0, 20, 32});
    reg("nightstand", ":resources/house_interior/Using-Deco/nightstand.png", {5, 8, 4, 12});
    reg("dining_table", ":resources/house_interior/Using-Deco/dining_table.png", {0, 16, 32, 22});
    reg("bookcase", ":resources/house_interior/Using-Deco/bookcase.png", {4, 20, 64, 32});
    reg("stairs", ":resources/house_interior/Using-Deco/stairs.png", {0, 0, 0, 0});
    reg("window", ":resources/house_interior/Using-Deco/window.png", {0, 0, 0, 0});

    reg("purple_carpet", ":resources/house_interior/Using-Deco/purple_carpet_circle.png", {0, 0, 0, 0});
    reg("red_carpet_rect", ":resources/house_interior/Using-Deco/red_carpet_rect.png", {0, 0, 0, 0});

    reg("sidewallr1", ":resources/map-assets/Tile2_17.png", {6, 0, 10, 32});
    reg("sidewallr2", ":resources/map-assets/sidewallr2.png", {6, 0, 10, 32});

    reg("sidewalll1", ":resources/map-assets/Tile2_30.png", {15, 0, 10, 32});
    reg("sidewalll2", ":resources/map-assets/sidewalll2.png", {16, 0, 10, 32});

    reg("walltr", ":resources/map-assets/walltr.png", {0, 31, 27, 1});
    reg("walltrb", ":resources/map-assets/walltrb.png", {6, 8, 21, 24});

    reg("walltl", ":resources/map-assets/walltl.png", {5, 30, 27, 2});
    reg("walltlb", ":resources/map-assets/walltlb.png", {6, 0, 24, 32});

    reg("wallbr", ":resources/map-assets/wallbr.png", {13, 0, 14, 32});
    reg("wallbrb", ":resources/map-assets/wallbrb.png", {0, 0, 27, 27});

    reg("sidegate1", ":resources/map-assets/sidegate1.png", {16, 0, 10, 32});
    reg("sidegate4", ":resources/map-assets/sidegate4.png", {16, 0, 10, 32});

    reg("brokenlt", ":resources/map-assets/brokenlt.png", {6, 0, 10, 32});
    reg("brokenlb", ":resources/map-assets/brokenlb.png", {6, 0, 6, 27});

    reg("brokent", ":resources/map-assets/brokent.png", {21, 9, 9, 23});
    reg("brokenb", ":resources/map-assets/brokenb.png", {4, 10, 28, 23});

    reg("brokenlog1", ":resources/map-assets/brokenlog1.png", {15, 8, 6, 24});
    reg("brokenlog2", ":resources/map-assets/brokenlog2.png", {10, 0, 8, 20});

    reg("remains1", ":resources/map-assets/remains1.png", {1, 15, 19, 13});
    reg("remains2", ":resources/map-assets/remains2.png", {11, 12, 11, 17});
    reg("remains3", ":resources/map-assets/remains3.png", {8, 9, 14, 11});
    reg("couch_side", ":resources/house_interior/Using-Deco/couch_side_profile.png", {10, 22, 24, 16});
    reg("cupboard_full", ":resources/house_interior/Using-Deco/cupboard_full.png", {0, 16, 50, 32});
    reg("cupboard_empty", ":resources/house_interior/Using-Deco/cupboard_empty.png", {0, 16, 42, 32});
    reg("wooden_crates", ":resources/house_interior/Using-Deco/wooden_crates.png", {9, 10, 10, 10});
    reg("round_table", ":resources/house_interior/Using-Deco/round_table.png", {4, 12, 34, 20});
    reg("sword_holder", ":resources/house_interior/Using-Deco/sword_holder.png", {0, 8, 60, 24});
    reg("potion_table", ":resources/house_interior/Using-Deco/potion_table.png", {2, 16, 28, 13});
    reg("wooden_barrel", ":resources/house_interior/Using-Deco/wooden_barrel.png", {4, 8, 30, 117});
    reg("wall_middle", ":resources/house_interior/Using/wall_middle.png", {0, 20, 36, 25});
    reg("wall_left", ":resources/house_interior/Using/wall_left.png", {0, 20, 36, 15});
    reg("wall_right", ":resources/house_interior/Using/wall_right.png", {-4, 20, 36, 15});
    reg("wall_right_turn", ":resources/house_interior/Using/wall_right_turn.png", {0, 0, 1, 32});
    reg("wall_left_turn", ":resources/house_interior/Using/wall_left_turn.png", {30, 0, 2, 32});
    reg("side_wall", ":resources/house_interior/Using/side_of_wall.png", {32, 0, 1, 130});
    reg("side_wall_2", ":resources/house_interior/Using/side_of_wall_2.png", {32, 20, 1, 96});

    reg("red_carpet_oval", ":resources/house_interior/Using-Deco/red_carpet_oval_2.png", {0, 0, 0, 0});
    reg("red_carpet_rect_2", ":resources/house_interior/Using-Deco/red_carpet_rectangle_2.png", {0, 0, 0, 0});
    reg("entrance_mat", ":resources/house_interior/Using/entrance.png", {0, 0, 0, 0});
    reg("wall_no_door", ":resources/house_interior/Using/wall_no_door.png", {0, 0, 0, 0});
    reg("banana", ":resources/house_interior/Using-Deco/banana.png", {0, 0, 0, 0});
    reg("garlic", ":resources/house_interior/Using-Deco/garlic.png", {0, 0, 0, 0});
    reg("coal", ":resources/house_interior/Using-Deco/coal.png", {0, 0, 0, 0});

    // map2 dungeon objects
    reg("3obj", ":resources/map-assets/map2 objects/3obj.png", {2, 2, 25, 28});
    reg("4obj", ":resources/map-assets/map2 objects/4obj.png", {2, 2, 27, 29});
    reg("openbox", ":resources/map-assets/map2 objects/openbox.png", {2, 2, 22, 18});
    reg("threeboxes", ":resources/map-assets/map2 objects/threeboxes.png", {2, 2, 28, 38});
    reg("torch,box1", ":resources/map-assets/map2 objects/torch,box1.png", {2, 2, 23, 26});
    reg("treasure", ":resources/map-assets/map2 objects/treasure.png", {2, 2, 20, 16});
    reg("treasureboxmon", ":resources/map-assets/map2 objects/treasureboxmon.png", {2, 2, 19, 19});
    reg("treasuremon", ":resources/map-assets/map2 objects/treasuremon.png", {2, 2, 26, 23});
    reg("twoboxes", ":resources/map-assets/map2 objects/twoboxes.png", {2, 2, 22, 27});
    reg("twoboxes1", ":resources/map-assets/map2 objects/twoboxes1.png", {2, 2, 21, 25});
    reg("dungeon_stone_slab", ":resources/map-assets/map2 objects/dungeon_stone_slab.png", {2, 2, 28, 20});
    reg("dungeon_coins", ":resources/map-assets/map2 objects/dungeon_coins.png", {2, 2, 24, 13});
    reg("dungeon_coins2", ":resources/map-assets/map2 objects/dungeon_coins2.png", {2, 2, 24, 13});
    reg("dungeon_gems_blue", ":resources/map-assets/map2 objects/dungeon_gems_blue.png", {2, 2, 24, 15});
    reg("dungeon_coins_flat", ":resources/map-assets/map2 objects/dungeon_coins_flat.png", {2, 2, 24, 6});
    reg("dungeon_gold_sm", ":resources/map-assets/map2 objects/dungeon_gold_sm.png", {2, 2, 12, 8});
    reg("dungeon_gem_blue", ":resources/map-assets/map2 objects/dungeon_gem_blue.png", {2, 2, 12, 8});
    reg("dungeon_coin", ":resources/map-assets/map2 objects/dungeon_coin.png", {2, 2, 10, 3});
    reg("dungeon_stone_sm", ":resources/map-assets/map2 objects/dungeon_stone_sm.png", {2, 2, 10, 15});
    reg("dungeon_barrel", ":resources/map-assets/map2 objects/dungeon_barrel.png", {2, 2, 24, 26});
    reg("dungeon_rubble", ":resources/map-assets/map2 objects/dungeon_rubble.png", {2, 2, 24, 12});
    reg("dungeon_stairs", ":resources/map-assets/map2 objects/dungeon_stairs.png", {2, 2, 31, 48});
    reg("dungeon_ladder", ":resources/map-assets/map2 objects/dungeon_ladder.png", {2, 2, 8, 48});
    reg("dungeon_crate", ":resources/map-assets/map2 objects/dungeon_crate.png", {2, 2, 28, 28});
    reg("dungeon_pot", ":resources/map-assets/map2 objects/dungeon_pot.png", {2, 2, 17, 22});
    reg("dungeon_vase_sm", ":resources/map-assets/map2 objects/dungeon_vase_sm.png", {2, 2, 10, 22});
    reg("dungeon_vase", ":resources/map-assets/map2 objects/dungeon_vase.png", {2, 2, 12, 22});
    reg("dungeon_barrel_blue", ":resources/map-assets/map2 objects/dungeon_barrel_blue.png", {2, 2, 26, 22});
    reg("dungeon_ladder2", ":resources/map-assets/map2 objects/dungeon_ladder2.png", {2, 2, 8, 53});

    // map3 objects
    reg("statue_1", ":resources/Level3-assets/tiles/tile609", {5, 10, 30, 30});
    reg("statue_2", ":resources/Level3-assets/tiles/tile610", {5, 10, 30, 30});

    assetsLoaded = true;
}

void MapLoader::drawBaseTiles()
{
    const QPixmap &bgPx = baseTileRegistry.value(backgroundTileId); // get the "base" tile for this map
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            auto *base = new QGraphicsPixmapItem(bgPx);
            scalefactor = (qreal)TILE_SIZE / bgPx.width();
            base->setScale(scalefactor);
            base->setPos(j * TILE_SIZE, i * TILE_SIZE);
            base->setZValue(-100.0);
            addItem(base);

            int id = mapData[i][j];
            if (id != 0 && baseTileRegistry.contains(id))
            {
                auto *tile = new QGraphicsPixmapItem(baseTileRegistry[id]);
                tile->setScale((qreal)TILE_SIZE / baseTileRegistry[id].width());
                tile->setPos(j * TILE_SIZE, i * TILE_SIZE);
                if (overheadTiles.contains(id))
                {
                    tile->setZValue((i * TILE_SIZE) + (TILE_SIZE * 3));
                }
                else if (solidTileIDs.contains(id))
                {
                    tile->setZValue(-50.0);
                }
                else
                {
                    tile->setZValue(-100.0);
                }
                addItem(tile);
            }
        }
    }
}

void MapLoader::drawFieldDecorations()
{
    if (!hasFieldDecorations || decoPool.empty())
    {
        return;
    }
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

void MapLoader::placeCollidable(float row, float col, const QString &templateId)
{

    if (!templateRegistry.contains(templateId))
        return;
    const CollidableTemplate &tmpl = templateRegistry[templateId];

    QPointF worldPos(col * TILE_SIZE, row * TILE_SIZE);

    auto *item = new QGraphicsPixmapItem(tmpl.texture);

    item->setScale(scalefactor);

    item->setPos(worldPos);

    item->setTransformationMode(Qt::FastTransformation);

    QRectF scaledHitbox(
        tmpl.hitbox.x() * scalefactor,
        tmpl.hitbox.y() * scalefactor,
        tmpl.hitbox.width() * scalefactor,
        tmpl.hitbox.height() * scalefactor);

    QRectF worldHitbox = scaledHitbox.translated(worldPos);
    item->setZValue(worldHitbox.bottom());

    addItem(item);
    if (worldHitbox.width() > 0)
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

    // 99 means dont place anything else here
    int wTiles = std::ceil((double)px.width() / TILE_SIZE);
    int hTiles = std::ceil((double)px.height() / TILE_SIZE);
    for (int i = (int)row; i < (int)row + hTiles; i++)
        for (int j = (int)col; j < (int)col + wTiles; j++)
            if (i >= 0 && i < MAP_ROWS && j >= 0 && j < MAP_COLS)
                mapData[i][j] = 99;
}

// eventually thisll be "addAnimatedObject"
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
    // remove from old scene if necessary
    if (player->scene())
    {
        player->scene()->removeItem(player);
    }
    addItem(player);
    player->setPos(col * TILE_SIZE, row * TILE_SIZE);
    player->setZValue(player->y() + player->pixmap().height());
}

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

void MapLoader::spawnEnemies(const QJsonObject &cfg)
{
    int count = cfg["count"].toInt(15);
    int rowMin = cfg["rowMin"].toInt(5);
    int rowMax = cfg["rowMax"].toInt(28);
    int colMin = cfg["colMin"].toInt(5);
    int colMax = cfg["colMax"].toInt(33);

    qDebug() << "[spawnEnemies] count=" << count << "rowMin=" << rowMin << "rowMax=" << rowMax;

    struct EnemyType
    {
        QString classType;
        int variant;
        int weight;
    };

    QJsonArray typeArray = cfg["types"].toArray();
    std::vector<EnemyType> types;
    int totalWeight = 0;

    for (const QJsonValue &t : typeArray)
    {
        QJsonObject to = t.toObject();
        EnemyType et;
        et.classType = to["class"].toString("Slime");
        et.variant = to["variant"].toInt(1);
        et.weight = to["weight"].toInt(1);
        types.push_back(et);
        totalWeight += et.weight;
    }
    if (types.empty() || totalWeight == 0)
        return;

    int placed = 0, attempts = 0, maxAttempts = count * 100;
    while (placed < count && attempts < maxAttempts)
    {
        attempts++;
        int r = QRandomGenerator::global()->bounded(rowMin, rowMax);
        int c = QRandomGenerator::global()->bounded(colMin, colMax);

        if (collisionMap[r][c] != 0 || mapData[r][c] == 98 || mapData[r][c] == 99)
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

        BaseEnemy *enemy = nullptr;

        if (chosen->classType == "Skeleton")
        {
            // enemy = new Skeleton(chosen->variant);
            int i = 5;
        }
        else if (chosen->classType == "Boss")
        {
            // enemy = new Boss(chosen->variant);
            int i = 5;
        }
        else if (chosen->classType == "Vampire")
        {
            enemy = new Vampire(chosen->variant);
        }
        else
        {
            enemy = new Slime(chosen->variant);
        }

        if (enemy)
        {
            enemy->setPos(c * TILE_SIZE, r * TILE_SIZE);
            enemy->setZValue(r * TILE_SIZE + TILE_SIZE * 4);
            enemy->setPlayer(player);
            addItem(enemy);
            connect(enemy, &BaseEnemy::enemyDied, this, &MapLoader::onEnemyDied);

            mapData[r][c] = 98;
            placed++;
        }
    }
    currentEnemyCount += placed;
}

void MapLoader::spawnSpecificEnemies(const QJsonArray &enemies)
{
    int placed = 0;

    for (const QJsonValue &val : enemies)
    {
        QJsonObject e = val.toObject();
        float r = e["row"].toDouble();
        float c = e["col"].toDouble();
        int variant = e["variant"].toInt(1);
        QString classType = e["class"].toString("Slime");
        BaseEnemy *enemy = nullptr;

        if (classType == "Skeleton")
        {
            // enemy = new Skeleton(variant);
        }
        else if (classType == "Boss")
        {
            enemy = new Boss(variant);
        }
        else if (classType == "Vampire")
        {
            enemy = new Vampire(variant);
        }
        else
        {
            enemy = new Slime(variant);
        }

        if (enemy)
        {
            enemy->setPos(c * TILE_SIZE, r * TILE_SIZE);
            enemy->setZValue(r * TILE_SIZE + TILE_SIZE * 4);
            enemy->setPlayer(player);
            addItem(enemy);
            connect(enemy, &BaseEnemy::enemyDied, this, &MapLoader::onEnemyDied);
            if (dynamic_cast<Boss *>(enemy))
            {
                connect(enemy, &BaseEnemy::healthChanged, this, &MapLoader::bossHealthChanged);
            }

            int intRow = static_cast<int>(r);
            int intCol = static_cast<int>(c);
            if (intRow >= 0 && intRow < MAP_ROWS && intCol >= 0 && intCol < MAP_COLS)
            {
                mapData[intRow][intCol] = 98;
            }
            placed++;
        }
    }

    currentEnemyCount += placed;
}

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

    int tileValue = collisionMap[row][col];

    if (tileValue == 1)
    {
        if (row >= 17 && row <= 19 && col >= 14 && col <= 16 && player->haskey(1))
        { // this is where the door is
            return false;
        }
        if (row >= 28 && row <= 30 && col >= 45 && col <= 46 && player->haskey(2))
        {
            return false; // second door
        }
        return true;
    }

    return false;
}

int MapLoader::getCurrentEnemyCount()
{
    return currentEnemyCount;
}
