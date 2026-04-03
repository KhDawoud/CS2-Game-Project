#ifndef MAP_H
#define MAP_H
#include <vector>
#include <QGraphicsScene>
class Map: public QGraphicsScene{
    std::vector<std::vector<int>> map;
    QPixmap grassPixmap;
    QPixmap LeftPathPixmap;
    QPixmap RightPathPixmap;
    QPixmap UpPathPixmap;
    QPixmap DownPathPixmap;
    QPixmap CenterPathPixmap;
    QPixmap UpRightPathPixmap;
    QPixmap RightUpPathPixmap;
    QPixmap LeftUpPathPixmap;
    QPixmap House1;
    QPixmap House2;
    QPixmap House3;
    QPixmap House4;
    QPixmap Tent1;
    QPixmap Tent2;
    QPixmap Tent3;
    QPixmap Tent4;
    QPixmap Grass1;
    QPixmap Grass2;
    QPixmap Grass3;
    QPixmap Grass4;
    QPixmap Grass5;
    QPixmap Grass6;
    QPixmap Stone1;
    QPixmap Stone2;
    QPixmap topgatel;
    QPixmap bottomgatel;
    QPixmap topgater;
    QPixmap bottomgater;
    QPixmap sidewalll1;
    QPixmap sidewalll2;
    QPixmap sidewallr1;
    QPixmap sidewallr2;
    QPixmap wallblt;
    QPixmap wallblb;
    QPixmap wallbrt;
    QPixmap wallbrb;
    QPixmap walltop1;
    QPixmap walltop2;
    QPixmap walltop3;
    QPixmap wallbottom1;
    QPixmap wallbottom2;
    QPixmap wallbottom3;
    QPixmap walltlt;
    QPixmap walltlb;
    QPixmap walltrt;
    QPixmap walltrb;
    QPixmap sidegate1;
    QPixmap sidegate2;
    QPixmap sidegate3;
    QPixmap sidegate4;


public:
    Map();
    void DrawMap();
    void AddBuilding(int, int, QPixmap);
    void AddObject(int,int,QPixmap);
    void DrawField();
    void ImageLoader();
};

#endif // MAP_H
