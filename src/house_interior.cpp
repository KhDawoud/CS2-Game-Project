#include "house_interior.hpp"
#include <QFile>
#include <QGraphicsPixmapItem>
#include <QRandomGenerator>
#include <algorithm>
#include <vector>

House_Interior::House_Interior()
{
    LoadMapFromCSV(":resources/house_interior/CSV/house_interior.csv");
    ImageLoader();
    DrawMap();

    // --- CARPETS (zValue = 0.1) ---
    PlaceEntity(5.5, 6, purple_carpet, 0.5, false);
    PlaceEntity(8,11,red_carpet_oval_2,0.5,false);
    //PlaceEntity(8,11,red_carpet_oval,0.5,false);
    PlaceEntity(2, 9.9,red_carpet_rectangle_2,0.5,false);
    PlaceEntity(2, 9,red_carpet_rectangle_2,0.5,false);
    PlaceEntity(2,8,red_carpet_rectangle_2,0.5,false);
    PlaceEntity(2,7,red_carpet_rectangle_2,0.5,false);
    PlaceEntity(2,6,red_carpet_rectangle_2,0.5,false);
    PlaceEntity(2,5.1,red_carpet_rectangle_2,0.5,false);

    PlaceEntity(3,9.9,red_carpet_rectangle,0.5,false);
    PlaceEntity(3,9,red_carpet_rectangle,0.5,false);
    PlaceEntity(3,8,red_carpet_rectangle,0.5,false);
    PlaceEntity(3,7,red_carpet_rectangle,0.5,false);
    PlaceEntity(3,6,red_carpet_rectangle,0.5,false);
    PlaceEntity(3,5.1,red_carpet_rectangle,0.5,false);


    PlaceEntity(8.85,11.8, entrance, 1, false);
    // --- DINING AREA (zValue = 2) ---
    PlaceEntity(6, 6.65, dining_table, 2, false);


    // --- BEDROOM AREA (Top Left, zValue = 2) ---
    PlaceEntity(2, 5.6, bed, 2, true);
    PlaceEntity(2, 5, nightstand, 2, true);

    PlaceEntity(1, 10, bookcase, 2, true);
    PlaceEntity(1, 9, bookcase, 2, true);
    PlaceEntity(2, 10, couch_side_profile, 2, true);
    PlaceEntity(1.5,8,window,2,false);
    PlaceEntity(1.5,7,window,2,false);

    // --- PANTRY / STORAGE (Top Right, zValue = 2) ---
    PlaceEntity(1, 13, cupboard_full, 2, true);
    PlaceEntity(4, 13, wooden_crates, 2, true);    // (If you meant to use 'chest' here, swap it)
    PlaceEntity(1, 11, cupboard_empty, 2, true);
    PlaceEntity(1, 12, banana, 2, true);
    PlaceEntity(1.5, 12, banana, 2, true);
    PlaceEntity(3, 12, round_table, 2, true);
    PlaceEntity(3, 12.3, coal, 2.5, true);
    PlaceEntity(3, 12, garlic, 2.5, true);

    // --- ARMORY (Bottom Right, zValue = 2) ---
    PlaceEntity(6, 11, sword_holder, 2.2, true);

    // --- ALCHEMY / ENTRANCE (Left Nook, zValue = 2) ---
    PlaceEntity(4, 2, stairs, 2, false);
    PlaceEntity(7.5, 1, potion_table, 2, true);
    PlaceEntity(8, 2, wooden_barrel, 2, true);

    //--- Walls ---
    PlaceEntity(4,5,wall_right_turn, 2.1, true);


    // --- Walls of Room 1 (zValue = 2.1) ---

    PlaceEntity(6, 2, wall_no_door, 2.1, true);
    //PlaceEntity(7, 2, wall_middle, 2, true);
    PlaceEntity(6, 1, wall_left, 2.1, true);
    PlaceEntity(6, 3, wall_right, 2.1, true);


    // --- Floors of Room 1 (zValue = 0.1) ---
    //PlaceEntity(7, 3, floor_right, 0.1, true);
    //PlaceEntity(8, 3, floor_bottom_right_corner, 0.1, true);
    PlaceEntity(6,3,side_of_wall_2, 2.1, true);

    // --- Walls of Room 2 (zValue = 2.1) ---

    PlaceEntity(5, 13, wall_no_door, 2.1, true);
    PlaceEntity(5, 12, wall_middle, 2.1, true);
    PlaceEntity(5, 11, wall_middle, 2.1, true);
    PlaceEntity(5, 14, wall_right, 2.1, true);
    PlaceEntity(5,10,wall_left_turn, 2.2, true);
    PlaceEntity(1.1,10,side_of_wall, 2.1, true);

    //CollisionMap to be completed
    DrawCollisionMap();
}

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
        QString line = in.readLine();
        QStringList values = line.split(',');

        std::vector<int> row;
        for (const QString &val : values) {
            // need to make to int
            row.push_back(val.trimmed().toInt());
        }
        map.push_back(row);
    }
    file.close();
}

void House_Interior::DrawMap()
{
    for (int i = 0; i < HOUSE_ROWS; i++) {
        for (int j = 0; j < HOUSE_COLS; j++) {
            // grass is always at z = 0
            QGraphicsPixmapItem *base = new QGraphicsPixmapItem(tileTextures[0]);
            base->setPos(j * SIZE, i * SIZE);
            addItem(base);

            int tileId = map[i][j];

            if (tileId != 0 && tileTextures.find(tileId) != tileTextures.end()) {
                QGraphicsPixmapItem *tile = new QGraphicsPixmapItem(tileTextures[tileId]);
                tile->setPos(j * SIZE, i * SIZE);

                if (tileId >= 2 && tileId <= 9) {
                    // paths, z = 0
                    tile->setZValue(0);
                } else if (tileId >= 10 && tileId <= 12) {
                    // walls of baselayer z = 0.1
                    tile->setZValue(1);
                }

                addItem(tile);
            }
        }
    }
}

void House_Interior::PlaceEntity(float startRow, float startCol, const QPixmap &image, qreal zValue, bool border)
{
    int widthInTiles = std::ceil((double) image.width() / SIZE);
    int heightInTiles = std::ceil((double) image.height() / SIZE);

    QGraphicsPixmapItem *entity = new QGraphicsPixmapItem(image);
    entity->setPos(startCol * SIZE, startRow * SIZE);
    entity->setZValue(zValue);
    addItem(entity);
    if (border) {
        for (int i = qRound(startRow); i < startRow + heightInTiles; i++) {
            for (int j = qRound(startCol); j < startCol + widthInTiles; j++) {
                if (i < HOUSE_ROWS && j < HOUSE_COLS) {
                    map[i][j] = 1; // Mark as occupied
                }
            }
        }
    }
}

void House_Interior::ImageLoader()
{
    //Black Space
    tileTextures[0].load(":resources/house_interior/Using/black_image.png");

    // Floor
    tileTextures[2].load(":resources/house_interior/Using/floor_left.png");
    tileTextures[3].load(":resources/house_interior/Using/floor_right.png");
    tileTextures[4].load(":resources/house_interior/Using/floor_bottom.png");
    tileTextures[5].load(":resources/house_interior/Using/floor_bottom_left_corner.png");
    tileTextures[6].load(":resources/house_interior/Using/floor_bottom_right_corner.png");
    tileTextures[7].load(":resources/house_interior/Using/floor_top_left_corner.png");
    tileTextures[8].load(":resources/house_interior/Using/floor_top_right_corner.png");
    tileTextures[9].load(":resources/house_interior/Using/floor_middle.png");
    //Walls

    tileTextures[10].load(":resources/house_interior/Using/wall_middle.png");
    tileTextures[11].load(":resources/house_interior/Using/wall_left.png");
    tileTextures[12].load(":resources/house_interior/Using/wall_right.png");

    //Carpets
    purple_carpet.load(":resources/house_interior/Using-Deco/purple_carpet_circle.png");
    //red_carpet.load(":resources/house_interior/Using-Deco/red_carpet_oval.png")
    red_carpet_oval_2.load(":resources/house_interior/Using-Deco/red_carpet_oval_2.png");
    red_carpet_rectangle.load(":resources/house_interior/Using-Deco/red_carpet_rectangle.png");
    red_carpet_rectangle_2.load(":resources/house_interior/Using-Deco/red_carpet_rectangle_2.png");
    entrance.load(":resources/house_interior/Using/entrance.png");

    // --- DINING AREA ---
    dining_table.load(":resources/house_interior/Using-Deco/dining_table.png");

    // --- BEDROOM AREA ---
    bed.load(":resources/house_interior/Using-Deco/bed.png");
    nightstand.load(":resources/house_interior/Using-Deco/nightstand.png");
    bookcase.load(":resources/house_interior/Using-Deco/bookcase.png");
    couch_side_profile.load(":resources/house_interior/Using-Deco/couch_side_profile.png");
    window.load(":resources/house_interior/Using-Deco/window.png");

    // --- PANTRY / STORAGE ---
    cupboard_full.load(":resources/house_interior/Using-Deco/cupboard_full.png");
    cupboard_empty.load(":resources/house_interior/Using-Deco/cupboard_empty.png");
    wooden_crates.load(":resources/house_interior/Using-Deco/wooden_crates.png");
    banana.load(":resources/house_interior/Using-Deco/banana.png");
    garlic.load(":resources/house_interior/Using-Deco/garlic.png");
    coal.load(":resources/house_interior/Using-Deco/coal.png");
    round_table.load(":resources/house_interior/Using-Deco/round_table.png");


    // --- ARMORY ---
    sword_holder.load(":resources/house_interior/Using-Deco/sword_holder.png");

    // --- ALCHEMY / ENTRANCE ---
    stairs.load(":resources/house_interior/Using-Deco/stairs.png");
    potion_table.load(":resources/house_interior/Using-Deco/potion_table.png");
    wooden_barrel.load(":resources/house_interior/Using-Deco/wooden_barrel.png");

    // --- WALL ---
    wall_no_door.load(":resources/house_interior/Using/wall_no_door.png");
    wall_middle.load(":resources/house_interior/Using/wall_middle.png");
    wall_left.load(":resources/house_interior/Using/wall_left.png");
    wall_right.load(":resources/house_interior/Using/wall_right.png");
    wall_right_turn.load(":resources/house_interior/Using/wall_right_turn.png");
    wall_left_turn.load(":resources/house_interior/Using/wall_left_turn.png");
    side_of_wall.load(":resources/house_interior/Using/side_of_wall.png");
    side_of_wall_2.load(":resources/house_interior/Using/side_of_wall_2.png");

    //--- Floor ---
    floor_left.load(":resources/house_interior/Using/floor_left.png");
    floor_bottom_left_corner.load(":resources/house_interior/Using/floor_bottom_left_corner.png");
    floor_right.load(":resources/house_interior/Using/floor_right.png");
    floor_bottom_right_corner.load(":resources/house_interior/Using/floor_bottom_right_corner.png");
    floor_top_left_corner.load(":resources/house_interior/Using/floor_top_left_corner.png");
}


void House_Interior::DrawCollisionMap()
{
    std::vector<std::vector<int>> collision_map(HOUSE_ROWS, std::vector<int>(HOUSE_COLS));
    for (int i = 0; i < HOUSE_ROWS; i++) {
        for (int j = 0; j < HOUSE_COLS; j++) {
            if ((map[i][j] > 9 && map[i][j] < 27) || map[i][j] == 1
                || map[i][j] > 29 && map[i][j] < 37) {
                collision_map[i][j] = 1;
            } else {
                collision_map[i][j] = 0;
            }
        }
    }
}

