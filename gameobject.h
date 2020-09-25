#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QImage>
#include <QRect>
#include <QString>

namespace CONSTANTS{
    const int SCREEN_WIDTH = 400;
    const int SCREEN_HEIGHT = 400;
    const int MARGIN_TOP = 64;

    const int TILE_ROW = 8;
    const int TILE_COL = 8;
    const int PATH_TILE_COUNT = 22;
    const int MAP[TILE_ROW*TILE_COL] = { 0, 0, 0, 0, 0, 0, 1, 0,
                                         0, 7, 6, 5, 4, 3, 2, 0,
                                         0, 8, 0, 0, 0, 0, 0, 0,
                                         0, 9, 0, 0,16,17,18, 0,
                                         0,10, 0, 0,15, 0,19, 0,
                                         0,11,12,13,14, 0,20, 0,
                                         0, 0, 0, 0, 0, 0,21, 0,
                                         0, 0, 0, 0, 0, 0,22, 0};
    const int TOWER_COST = 10;
}

class GameObject : public QObject
{
    Q_OBJECT
public:

    GameObject();
    GameObject(QString, qreal=1);
    virtual ~GameObject();


      QRect* getRect(){ return rect; }
      QImage* getImage() { return image; }
      QRect getRectV() const { return *rect; }
      QImage getImageV() const { return *image; }

      void setImage(QImage i) { image = new QImage(i); }
      void setRect(QRect r) { rect = new QRect(r); }
private:
    QImage* image;
    QRect* rect;
};

#endif
