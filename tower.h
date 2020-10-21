#ifndef TOWER_H
#define TOWER_H

#include "gameobject.h"
#include <QPointF>
#include "image.h"
#include "enemy.h"




enum Type{FIRE,ICE,EARTH};


class Tower : public GameObject
{
    Q_OBJECT
public:

    Tower(QString fileName, QRect tile);


      int getTimer() const { return timerID; }
      bool isCoolDown() const { return coolDown; }
      Type getType() const { return type; }


      void setTimer(int id) { timerID = id; }
      void setCoolDown(bool c) { coolDown = c; }


    static int getCost(Type t);
    static int getDamageCost(Type t);
    static int getRangeCost(Type t);
    static int getCoolDownCost(Type t);
    static int getDamage(Type t);
    static int getRange(Type t);
    static int getCoolDown(Type t);
    static void upgradeDamage(Type t);
    static void upgradeRange(Type t);
    static void upgradeCoolDown(Type t);


    static void resetUpgrades();
public slots:
    void toggleCoolDown(){setCoolDown(false);}
private:

    Type type;
    int timerID;
    bool coolDown;


    class TowerStats{
    public:
        TowerStats():d_count(0), r_count(0), s_count(0){}
        int d_count, r_count, s_count;
    };


    static TowerStats fire;
    static TowerStats ice;
    static TowerStats earth;


    static int fireCount;
    static int iceCount;
    static int earthCount;
};

#endif
