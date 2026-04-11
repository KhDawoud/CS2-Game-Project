/*#ifndef HOUSE_INTERIOR_HPP
#define HOUSE_INTERIOR_HPP


#include <QGraphicsScene>
#include <QPixmap>
#include <unordered_map>
#include <vector>
#include <QTimer>
#include "campfire.hpp"
constexpr int HOUSE_ROWS = 10;
constexpr int HOUSE_COLS = 16;
constexpr int SIZE = 32;

class House_Interior : public QGraphicsScene
{
    std::vector<std::vector<int>> map;
    std::unordered_map<int, QPixmap> tileTextures;
    // --- CARPET ---
    QPixmap purple_carpet;
    QPixmap red_carpet_oval;
    QPixmap red_carpet_oval_2;
    QPixmap red_carpet_rectangle;
    QPixmap red_carpet_rectangle_2;
    QPixmap entrance;

    // --- DINING AREA ---
    QPixmap dining_table;


    // --- BEDROOM AREA ---
    QPixmap bed;
    QPixmap nightstand;
    QPixmap bookcase;
    QPixmap couch_side_profile;
    QPixmap window;

    // --- PANTRY / STORAGE ---
    QPixmap cupboard_full;
    QPixmap cupboard_empty;
    QPixmap wooden_crates;
    QPixmap garlic;
    QPixmap banana;
    QPixmap coal;
    QPixmap round_table;

    // --- ARMORY ---
    QPixmap sword_holder;

    // --- ALCHEMY / ENTRANCE ---
    QPixmap stairs;
    QPixmap potion_table;
    QPixmap wooden_barrel;

    //--- Walls ---
    QPixmap wall_no_door;
    QPixmap wall_middle;
    QPixmap wall_left;
    QPixmap wall_right;
    QPixmap wall_right_turn;
    QPixmap wall_left_turn;
    QPixmap side_of_wall;
    QPixmap side_of_wall_2;

    //--- Floor ---
    QPixmap floor_right;
    QPixmap floor_bottom_right_corner;
    QPixmap floor_left;
    QPixmap floor_bottom_left_corner;
    QPixmap floor_top_left_corner;
    QPixmap floor_top_right_corner;

public:
    House_Interior();
    void LoadMapFromCSV(const QString &filePath);
    void DrawMap();
    void PlaceEntity(float, float, const QPixmap &, qreal, bool border);
    void DrawField();
    void ImageLoader();
    std::vector<std::vector<int>> collision_map;
    void DrawCollisionMap();
    void AddTrees();
    void AddCampfire(float, float, Campfire*);
};

#endif // HOUSE_INTERIOR_HPP
*/
