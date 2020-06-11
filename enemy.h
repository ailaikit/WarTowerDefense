#ifndef ENEMY_H
#define ENEMY_H

#include "gameobject.h"
#include "image.h"




enum class Enemy_Type{NORMAL, BADASS, BAT};


namespace ENEMY {
    const QString NORMAL_L = "white ghost left.png";
    const QString NORMAL_R = "white ghost right.png";
    const QString BADASS_L = "red ghost left.png";
    const QString BADASS_R = "red ghost right.png";
    const QString BAT_L = "bat_l.png";
    const QString BAT_R = "bat_r.png";
}

class Enemy : public GameObject
{
public:
    Enemy(Enemy_Type type, QPointF p);
    ~Enemy();


    void move(QPointF w);


      int getCurWaypoint() const { return currentWaypoint; }


      void incrementCurWaypoint() { currentWaypoint++; }


      void inflictDamage(int d) { health -= d; }


      bool isDead() const { return dead; }


      int getHealth() const { return health; }


      void setDead(bool b) { dead = b; }


      int getScore() const { return score; }


      int getSpawnDelay() const { return spawnDelay; }


    Image getAnimation() const;
private:

    int currentWaypoint;


    int health;
    bool dead;
    int score;
    int spawnDelay;


    bool faceRight;
    Image* rightAnimation;
    Image* leftAnimation;


    void moveLeft(int& x);
    void moveRight(int& x);
};

#endif
