#include "game.h"
#include "waypoint.h"
#include "enemy.h"
#include "wavegenerator.h"

#include <QApplication>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>

Game::Game(QWidget *parent) : QWidget(parent) , state(MENU) , curTowerOpt(0), curTowerType(FIRE), wave_value(0), score_value(10) ,
    enemyCount(0), generator(SEED), damageDisplayOffset(-2,2), tooltip(NULL)
{
    setWindowTitle("Warmeme");
    setFixedSize(CONSTANTS::SCREEN_WIDTH, CONSTANTS::SCREEN_HEIGHT);

    setMouseTracking(true);

    fillCharReferences();
    loadMenu();
    loadPause();
    loadInGame();
}

Game::~Game()
{
    cleanMenu();
    cleanPause();
    cleanInGame();
    cleanCharReferences();
}

void Game::cleanCharReferences(){
    for(auto& c : letterChars)
        delete c;
    for(auto& c : letterCharsAct)
        delete c;
    for(auto& c : letterCharsRed)
        delete c;
    for(auto& c : specialChars)
        delete c;
}

void Game::paintEvent(QPaintEvent*){
    QPainter painter(this);

    switch(state){
        case MENU:
            painter.drawImage(*title_line1->getRect(), *title_line1->getImage());
            painter.drawImage(*title_line2->getRect(), *title_line2->getImage());

            if(start_button->isActive())
                painter.drawImage(*start_button->getRect(), start_button->getActiveImage());
            else
                painter.drawImage(*start_button->getRect(), *start_button->getImage());
            if(quit_button->isActive())
                painter.drawImage(*quit_button->getRect(), quit_button->getActiveImage());
            else
                painter.drawImage(*quit_button->getRect(), *quit_button->getImage());
            break;
        case INGAME:
            paintChar(std::to_string(getWave()),1,painter,10,10+wave_title->getRect()->height(),false);
            painter.drawImage(*score_title->getRect(),*score_title->getImage());
            paintChar(std::to_string(getScore()),1,painter,width()-std::to_string(getScore()).length()*6-5, 10+score_title->getRect()->height(),false);
            painter.drawImage(*wave_title->getRect(),*wave_title->getImage());

            for(const auto o : towerOptions)
                painter.drawImage(*o->getRect(), *o->getImage());
            painter.drawImage(*towerOptions[curTowerOpt]->getRect(), *towerOptHighlight->getImage());

            switch(curTowerOpt){
                case 0:
                    for(auto& u : fire_upgrade)
                        painter.drawImage(*u->getRect(), *u->getImage());
                    break;
                case 1:
                    for(auto& u : ice_upgrade)
                        painter.drawImage(*u->getRect(), *u->getImage());
                    break;
                case 2:
                    for(auto& u : earth_upgrade)
                        painter.drawImage(*u->getRect(), *u->getImage());
                    break;
            }

            for(auto& i : upgrade_icon)
                painter.drawImage(*i->getRect(), *i->getImage());

            for(auto& t : map){
                painter.drawImage(*t->getRect(), *t->getImage());
                if(t->isActive())
                    painter.drawImage(*tileHighlight->getRect(), *tileHighlight->getImage());
            }

            for(auto& e : enemies){
                if(!e->isDead())
                    painter.drawImage(*e->getRect(), *e->getImage());
            }
\
            //Draw the towers
            for(const auto t : towers)
                painter.drawImage(*t->getRect(), *t->getImage());

            for(const auto d : damageDisplays)
                painter.drawImage(*d->getRect(), *d->getImage());

            if(tooltip != NULL)
                tooltip->paint(&painter);
            break;
        case CLEARED:
            paintChar("wave "+std::to_string(getWave())+" cleared",0.25,painter,(width()-(13+std::to_string(getWave()).length())*20)/2,100,false);

            if(continue_button->isActive())
                painter.drawImage(*continue_button->getRect(), continue_button->getActiveImage());
            else
                painter.drawImage(*continue_button->getRect(), *continue_button->getImage());
            break;
        case PAUSED:
            for(const auto b : pauseButtons){
                if(b->isActive())
                    painter.drawImage(*b->getRect(), b->getActiveImage());
                else
                    painter.drawImage(*b->getRect(), *b->getImage());
            }
            break;
    }
}

void Game::timerEvent(QTimerEvent *event){
    if(state == INGAME){
        if(event->timerId() == spawnTimer)
            spawner();
    }
    repaint();
}

void Game::spawner(){
    killTimer(spawnTimer);

    if(!spawnList.empty()){
        enemies.push_back(spawnList.back());
        spawnTimer = startTimer(spawnList.back()->getSpawnDelay());
        spawnList.pop_back();
    }
}

void Game::moveEnemies(){
    for(auto& e : enemies){
        if(e->getRect()->contains(navPath[CONSTANTS::PATH_TILE_COUNT - 1].toPoint())){
            state = MENU;
            break;
        }
        if(e->getRect()->contains(navPath[e->getCurWaypoint()+1].toPoint()))
        {
            e->incrementCurWaypoint();
        }
        e->move(navPath[e->getCurWaypoint()+1]);
    }
}

void Game::keyPressEvent(QKeyEvent* event){
    if(state == INGAME){
        switch(event->key()){
            case Qt::Key_P:
                    state = PAUSED;
                    break;
            case Qt::Key_Escape:
                    qApp->exit();
                    break;
            default:
                QWidget::keyPressEvent(event);
        }
    }
    else
        QWidget::keyPressEvent(event);
}

void Game::mouseMoveEvent(QMouseEvent *event){
    switch(state){
        case MENU:
            if(start_button->getRect()->contains(event->pos())){
                start_button->setActive(true);
                quit_button->setActive(false);
            }
            else if(quit_button->getRect()->contains(event->pos())){
                quit_button->setActive(true);
                start_button->setActive(false);
            }
            else{
                start_button->setActive(false);
                quit_button->setActive(false);
            }
            break;
        case PAUSED:
            if(pauseButtons[0]->getRect()->contains(event->pos())){
                pauseButtons[0]->setActive(true);
                pauseButtons[1]->setActive(false);
            }
            else if(pauseButtons[1]->getRect()->contains(event->pos())){
                pauseButtons[0]->setActive(false);
                pauseButtons[1]->setActive(true);
            }
            else{
                pauseButtons[0]->setActive(false);
                pauseButtons[1]->setActive(false);
            }
            break;
        case CLEARED:
            if(continue_button->getRect()->contains(event->pos()))
                continue_button->setActive(true);
            else
                continue_button->setActive(false);
            break;
        case INGAME:
            delete tooltip;
            tooltip = NULL;


            if(towerOptions[0]->getRect()->contains(event->pos())){
                tooltip = new ToolTip(mergeChars("cost", 1, NORMAL),
                                      mergeChars(std::to_string(Tower::getCost(FIRE)), 1, ACTIVE));
                tooltip->moveTo(event->pos());
            }

            else if(towerOptions[1]->getRect()->contains(event->pos())){
                tooltip = new ToolTip(mergeChars("cost", 1, NORMAL),
                                      mergeChars(std::to_string(Tower::getCost(ICE)), 1, ACTIVE));
                tooltip->moveTo(event->pos());
            }

            else if(towerOptions[2]->getRect()->contains(event->pos())){
                tooltip = new ToolTip(mergeChars("cost", 1, NORMAL),
                                      mergeChars(std::to_string(Tower::getCost(EARTH)), 1, ACTIVE));
                tooltip->moveTo(event->pos());
            }

            if(upgrade_icon[0]->getRect()->contains(event->pos())){
                tooltip = new ToolTip(mergeChars("cost", 1, NORMAL),
                                      mergeChars(std::to_string(Tower::getDamageCost(curTowerType)), 1, ACTIVE),
                                      mergeChars("str", 1, NORMAL),
                                      mergeChars(std::to_string(Tower::getDamage(curTowerType)), 1, ACTIVE));
                tooltip->moveTo(event->pos());
            }
            else if(upgrade_icon[1]->getRect()->contains(event->pos())){
                tooltip = new ToolTip(mergeChars("cost", 1, NORMAL),
                                      mergeChars(std::to_string(Tower::getRangeCost(curTowerType)), 1, ACTIVE),
                                      mergeChars("range", 1, NORMAL),
                                      mergeChars(std::to_string(Tower::getRange(curTowerType)), 1, ACTIVE));
                tooltip->moveTo(event->pos());
            }
            else if(upgrade_icon[2]->getRect()->contains(event->pos())){
               tooltip = new ToolTip(mergeChars("cost", 1, NORMAL),
                                      mergeChars(std::to_string(Tower::getCoolDownCost(curTowerType)), 1, ACTIVE),
                                      mergeChars("rate", 1, NORMAL),
                                      mergeChars(std::to_string(Tower::getCoolDown(curTowerType)), 1, ACTIVE));
                tooltip->moveTo(event->pos());
            }
            break;
    }
    repaint();
}


void Game::mousePressEvent(QMouseEvent *event){
    switch(state){
        case MENU:
            if(start_button->getRect()->contains(event->pos())){
                state = INGAME;
                newGame();
            }
            else if(quit_button->getRect()->contains(event->pos())){
                qApp->quit();
            }
            break;
        case PAUSED:
            if(pauseButtons[0]->getRect()->contains(event->pos())){
                state = INGAME;
                startTimers();

            }
            else if(pauseButtons[1]->getRect()->contains(event->pos())){
                killTimer(paintTimer);
                state = MENU;
            }
            break;
    case INGAME:
        for(auto& t : map)
            (!t->isPath() && !t->isOccupied() && t->getRect()->contains(event->pos())) ? selectTile(t) : t->setActive(false);

        for(size_t i=0; i<towerOptions.size(); i++){
            if(towerOptions[i]->getRect()->contains(event->pos())){
                curTowerOpt = i;
                switch(curTowerOpt){
                    case 0:
                        curTowerType = FIRE;
                        break;
                    case 1:
                        curTowerType = ICE;
                        break;
                    case 2:
                        curTowerType = EARTH;
                        break;
                }
            }
        }


        if(getScore() > Tower::getDamageCost(curTowerType) && upgrade_icon[0]->getRect()->contains(event->pos())){
            updateScore(-Tower::getDamageCost(curTowerType));
            Tower::upgradeDamage(curTowerType);
            tooltip = new ToolTip(mergeChars("cost", 1, NORMAL),
                                  mergeChars(std::to_string(Tower::getDamageCost(curTowerType)), 1, ACTIVE),
                                  mergeChars("str", 1, NORMAL),
                                  mergeChars(std::to_string(Tower::getDamage(curTowerType)), 1, ACTIVE));
            tooltip->moveTo(event->pos());
        }
        else if(getScore() > Tower::getRangeCost(curTowerType) && upgrade_icon[1]->getRect()->contains(event->pos())){
            updateScore(-Tower::getRangeCost(curTowerType));
            Tower::upgradeRange(curTowerType);
            tooltip = new ToolTip(mergeChars("cost", 1, NORMAL),
                                  mergeChars(std::to_string(Tower::getRangeCost(curTowerType)), 1, ACTIVE),
                                  mergeChars("range", 1, NORMAL),
                                  mergeChars(std::to_string(Tower::getRange(curTowerType)), 1, ACTIVE));
            tooltip->moveTo(event->pos());
        }
        else if(getScore() > Tower::getCoolDownCost(curTowerType) && upgrade_icon[2]->getRect()->contains(event->pos())){
            updateScore(-Tower::getCoolDownCost(curTowerType));
            Tower::upgradeCoolDown(curTowerType);
            tooltip = new ToolTip(mergeChars("cost", 1, NORMAL),
                                  mergeChars(std::to_string(Tower::getCoolDownCost(curTowerType)), 1, ACTIVE),
                                  mergeChars("rate", 1, NORMAL),
                                  mergeChars(std::to_string(Tower::getCoolDown(curTowerType)), 1, ACTIVE));
            tooltip->moveTo(event->pos());
        }

        break;
    case CLEARED:
        if(continue_button->getRect()->contains(event->pos())){
            newWave(); //start next wave
            state = INGAME;
        }
        break;
    }
}

void Game::newGame(){
    clearGame();
    wave_value = 0;
    newWave();
    score_value = 20;
    paintTimer = startTimer(10);
}

void Game::startTimers(){
    QTimer::singleShot(30,this,SLOT(moveEvent()));
    QTimer::singleShot(150,this,SLOT(moveDecals()));
    QTimer::singleShot(30,this,SLOT(collisionEvent()));
}

void Game::newWave(){
    updateWave();
    for(auto& e : enemies)
        delete e;
    enemies.clear();

    spawnList.clear();

    spawnList = wave_generator.generateSpawnList(getWave(), navPath[0]);
    enemyCount = spawnList.size();

    spawnTimer = startTimer(2000);
    startTimers();
}

void Game::clearGame(){
    Tower::resetUpgrades();

    for(auto& e : enemies)
        delete e;
    enemies.clear();
    for(auto& t : towers)
        delete t;
    towers.clear();
    for(auto& t : map){
        t->setOccupied(false);
    }
    spawnList.clear();
}

void Game::loadMenu(){
    title_line1 = mergeChars("tower",0.125,NORMAL);
    title_line2 = mergeChars("defense",0.125,NORMAL);
    start_button = new Button(mergeChars("start",0.25,NORMAL), mergeChars("start",0.25,ACTIVE));
    quit_button = new Button(mergeChars("quit",0.25,NORMAL), mergeChars("quit",0.25,ACTIVE));

    int const top_margin = (height() - (title_line1->getRect()->height() + title_line2->getRect()->height() +
                           start_button->getRect()->height() +
                           quit_button->getRect()->height()))/2;

    title_line1->getRect()->moveTo( (width()-title_line1->getRect()->width())/2 , top_margin );
    title_line2->getRect()->moveTo( (width()-title_line2->getRect()->width())/2 , top_margin + title_line1->getRect()->height());
    start_button->getRect()->moveTo( (width()-start_button->getRect()->width())/2 , top_margin + title_line1->getRect()->height() + title_line2->getRect()->height());
    quit_button->getRect()->moveTo( (width()-quit_button->getRect()->width())/2 , top_margin + title_line1->getRect()->height() + title_line2->getRect()->height() + start_button->getRect()->height());
}

void Game::cleanMenu(){
    delete title_line1;
    delete title_line2;
    delete start_button;
    delete quit_button;
}

void Game::loadInGame(){
    score_title = mergeChars("score",1,NORMAL);
    wave_title = mergeChars("wave",1,NORMAL);
    tileHighlight = new Image(":/img/tile_highlight.png");
    towerOptions.push_back(new Image(":/img/fire.png"));
    towerOptions.push_back(new Image(":/img/ice.png"));
    towerOptions.push_back(new Image(":/img/rock.png"));
    towerOptHighlight = new Image(":/img/toweroption_h.png");

    for(int i = 0; i<3; i++){
        fire_upgrade.push_back(new Image(":/img/fire_icon_base.png"));
        ice_upgrade.push_back(new Image(":/img/ice_icon_base.png"));
        earth_upgrade.push_back(new Image(":/img/rock_icon_base.png"));
    }
    upgrade_icon.push_back(new Image(":/img/strength_icon.png"));
    upgrade_icon.push_back(new Image(":/img/target_icon.png"));
    upgrade_icon.push_back(new Image(":/img/time_icon.png"));

    continue_button = new Button(mergeChars("continue",0.25,NORMAL), mergeChars("continue",0.25,ACTIVE));

    wave_title->getRect()->moveTo(10,10);
    score_title->getRect()->moveTo(width()-score_title->getRect()->width()-5, 10);
    towerOptions[0]->getRect()->moveTo(width()-towerOptions[0]->getRect()->width()-5, 50);
    towerOptions[1]->getRect()->moveTo(width()-towerOptions[1]->getRect()->width()-5, 50 + towerOptions[0]->getRect()->height());
    towerOptions[2]->getRect()->moveTo(width()-towerOptions[2]->getRect()->width()-5, 50 + towerOptions[0]->getRect()->height() + towerOptions[1]->getRect()->height());

    int x = width()-towerOptions[0]->getRect()->width()-5;
    int y = 75 + towerOptions[0]->getRect()->height() + towerOptions[1]->getRect()->height() + towerOptions[2]->getRect()->height();
    for(size_t i = 0, s = fire_upgrade.size(); i < s; i++){
        fire_upgrade[i]->getRect()->moveTo(x+(fire_upgrade[i]->getRect()->width())/4, y);
        ice_upgrade[i]->getRect()->moveTo(x+(fire_upgrade[i]->getRect()->width())/4, y);
        earth_upgrade[i]->getRect()->moveTo(x+(fire_upgrade[i]->getRect()->width())/4, y);
        upgrade_icon[i]->getRect()->moveTo(x+(fire_upgrade[i]->getRect()->width())/4, y);
        y+= fire_upgrade[i]->getRect()->height()+2;
    }

    continue_button->getRect()->moveTo( (width()-continue_button->getRect()->width())/2 , 264);

    buildMap();
    createNavigationPath();
}

void Game::fillCharReferences(){
    letterChars.push_back(new Image(":/img/characters/Normal/0.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/1.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/2.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/3.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/4.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/5.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/6.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/7.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/8.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/9.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/A.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/B.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/C.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/D.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/E.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/F.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/G.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/H.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/I.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/J.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/K.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/L.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/M.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/N.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/O.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/P.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/Q.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/R.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/S.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/T.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/U.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/V.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/W.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/X.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/Y.png"));
    letterChars.push_back(new Image(":/img/characters/Normal/Z.png"));

    letterCharsAct.push_back(new Image(":/img/characters/Active/0.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/1.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/2.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/3.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/4.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/5.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/6.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/7.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/8.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/9.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/A.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/B.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/C.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/D.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/E.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/F.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/G.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/H.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/I.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/J.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/K.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/L.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/M.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/N.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/O.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/P.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/Q.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/R.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/S.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/T.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/U.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/V.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/W.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/X.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/Y.png"));
    letterCharsAct.push_back(new Image(":/img/characters/Active/Z.png"));

    letterCharsRed.push_back(new Image(":/img/characters/Red/0.png"));
    letterCharsRed.push_back(new Image(":/img/characters/Red/1.png"));
    letterCharsRed.push_back(new Image(":/img/characters/Red/2.png"));
    letterCharsRed.push_back(new Image(":/img/characters/Red/3.png"));
    letterCharsRed.push_back(new Image(":/img/characters/Red/4.png"));
    letterCharsRed.push_back(new Image(":/img/characters/Red/5.png"));
    letterCharsRed.push_back(new Image(":/img/characters/Red/6.png"));
    letterCharsRed.push_back(new Image(":/img/characters/Red/7.png"));
    letterCharsRed.push_back(new Image(":/img/characters/Red/8.png"));
    letterCharsRed.push_back(new Image(":/img/characters/Red/9.png"));

    specialChars.push_back(new Image(":/img/characters/space.png"));
}

void Game::cleanInGame(){
    delete score_title;
    delete wave_title;
    delete tileHighlight;
    delete tooltip;
    for(auto& t : map)
        delete t;
    for(auto& o : towerOptions)
        delete o;
    for(auto& d : damageDisplays)
        delete d;
    for(auto& e : enemies)
        delete e;
}

void Game::loadPause(){
    pauseButtons.push_back(new Button(mergeChars("resume",0.25,NORMAL), mergeChars("resume",0.25,ACTIVE)));
    pauseButtons.push_back(new Button(mergeChars("main menu",0.25,NORMAL), mergeChars("main menu",0.25,ACTIVE)));

    int const top_margin = (height() - (pauseButtons[0]->getRect()->height() + pauseButtons[1]->getRect()->height()))/2;
    pauseButtons[0]->getRect()->moveTo( (width()-pauseButtons[0]->getRect()->width())/2 , top_margin);
    pauseButtons[1]->getRect()->moveTo( (width()-pauseButtons[1]->getRect()->width())/2 , top_margin+pauseButtons[0]->getRect()->height());
}

void Game::cleanPause(){
    for(auto& b : pauseButtons)
        delete b;
}

void Game::buildMap(){
    for(const auto d : CONSTANTS::MAP)
        d==0 ?  map.push_back(new Tile(":/img/grass_tile.png")) : map.push_back(new Tile(":/img/dirt_tile.png",d));

    int xPos = 50;
    int yPos = 50;

    int colCounter = 0;

    for(auto& t : map){
        t->getRect()->moveTo(xPos, yPos);

        if(++colCounter<CONSTANTS::TILE_COL)
            xPos += t->getRect()->width();

        else{
            xPos = 50;
            colCounter = 0;
            yPos += t->getRect()->height();
        }
    }
}

void Game::selectTile(Tile* t){

    if(!t->isActive()){
        t->setActive(true);
        tileHighlight->getRect()->moveTo(t->getRect()->topLeft());
    }

    else{
        t->setActive(false);
        switch(curTowerOpt){
            case 0:
                if(getScore() >= Tower::getCost(curTowerType)){
                    updateScore(-Tower::getCost(curTowerType));
                    towers.push_back(new Tower(":/img/fire.png", *t->getRect()));
                    t->setOccupied(true);
                }
                break;
            case 1:
                if(getScore() >= Tower::getCost(curTowerType)){
                    updateScore(-Tower::getCost(curTowerType));
                    towers.push_back(new Tower(":/img/ice.png", *t->getRect()));
                    t->setOccupied(true);
                }
                break;
            case 2:
                if(getScore() >= Tower::getCost(curTowerType)){
                    updateScore(-Tower::getCost(curTowerType));
                    towers.push_back(new Tower(":/img/rock.png", *t->getRect()));
                    t->setOccupied(true);
                }
                break;
        }
    }
}

void Game::raycast(){
    for(auto& t : towers){
        for(auto& e : enemies){
            int distance = QLineF(t->getRect()->center(), e->getRect()->center()).length();
            if(distance < t->getRange(t->getType()) && !t->isCoolDown()){
                t->setCoolDown(true);
                QTimer::singleShot(t->getCoolDown(t->getType()),t,SLOT(toggleCoolDown()));

                e->inflictDamage(t->getDamage(t->getType()));

                Image* damage = mergeChars(std::to_string(t->getDamage(t->getType())),1,RED);

                damage->getRect()->moveTo(e->getRect()->center().x()+damageDisplayOffset(generator), e->getRect()->top());
                damageDisplays.push_back(damage);

                QTimer::singleShot(1000,this,SLOT(removeDecal()));

                if(e->getHealth() <= 0){
                    e->setDead(true);
                    enemyCount--;
                    cleanEnemyList();
                    if(enemyCount == 0)
                        state = CLEARED;
                }
                break;
            }
        }
    }
}

void Game::cleanEnemyList(){
    for(size_t i = 0; i<enemies.size(); i++){
        if(enemies[i]->isDead()){
            updateScore(enemies[i]->getScore());
            delete enemies[i];
            enemies.erase(enemies.begin()+i);
        }
    }
}

Image* Game::mergeChars(std::string word, double scale, Chars c){
    Image* image = new Image();

    for(size_t i = 0; i < word.length(); i++){

        if(c == ACTIVE){
            if ((int)(word[i]) >= 97) appendChar(letterCharsAct[((int)(word[i]))-87], scale, image);
            else if ((int)(word[i]) >= 48) appendChar(letterCharsAct[((int)(word[i]))-48], scale, image);

        }
        else if(c == NORMAL){
            if ((int)(word[i]) >= 97) appendChar(letterChars[((int)(word[i]))-87], scale, image);
            else if ((int)(word[i]) >= 48) appendChar(letterChars[((int)(word[i]))-48], scale, image);
                else if ((int)(word[i] = 32)) appendChar(specialChars[0], scale, image);
            }
        else if(c == RED){
            if ((int)(word[i]) >= 48) appendChar(letterCharsRed[((int)(word[i]))-48], scale, image);
            }
    }

    return image;
}

void Game::appendChar(Image* character, double scale, Image* i){
    Image* copy = character->scaledCopy(scale);
    i->append(copy);
}

void Game::printChar(Image* character, double scale, QPainter& p, int& x, int& y){
    Image* copy = character->scaledCopy(scale);
    copy->getRect()->moveTo(x,y);
    p.drawImage(*copy->getRect(),*copy->getImage());
    x += copy->getRect()->width();
    delete copy;
}

void Game::paintChar(std::string word, double scale, QPainter& p, int x, int y, bool active){
    for(size_t i = 0; i < word.length(); i++){
        if(active){
            if ((int)(word[i]) >= 97) printChar(letterCharsAct[((int)(word[i]))-87], scale, p, x, y);
            else if ((int)(word[i]) >= 48) printChar(letterCharsAct[((int)(word[i]))-48], scale, p, x, y);
            else if ((int)(word[i] = 32)) printChar(specialChars[0], scale, p, x, y);
        }
        else{
            if ((int)(word[i]) >= 97) printChar(letterChars[((int)(word[i]))-87], scale, p, x, y);
            else if ((int)(word[i]) >= 48) printChar(letterChars[((int)(word[i]))-48], scale, p, x, y);
                else if ((int)(word[i] = 32)) printChar(specialChars[0], scale, p, x, y);
         }
    }

}

void Game::createNavigationPath(){
    for(auto& t : map){
        if(t->isPath())
            navPath[t->getPathID()-1] = t->getRect()->center();
    }
}

Game::ToolTip::ToolTip(Image* s, Image* s_u, Image* c, Image* c_a) : upgrade(true)
{
    cost = c;
    cost_amount = c_a;
    stat = s;
    stat_upgrade = s_u;
    background = new Image(":/img/tooltip_base.png");
}

Game::ToolTip::ToolTip(Image* c, Image* c_a) : upgrade(false)
{
    stat = c;
    stat_upgrade = c_a;
    background = new Image(":/img/tooltip_base.png");
}

Game::ToolTip::~ToolTip(){
    delete background;
    delete stat;
    delete stat_upgrade;
    if(upgrade){
        delete cost;
        delete cost_amount;
    }
}

void Game::ToolTip::moveTo(QPointF position){
    int x = position.x();
    int y = position.y();
    resizeBackground();
    background->getRect()->moveTo(x-background->getRect()->width(), y);
    stat->getRect()->moveTo(background->getRect()->x()+2, background->getRect()->y()+2);
    stat_upgrade->getRect()->moveTo(stat->getRect()->right()+3, stat->getRect()->y());
    if(upgrade){
        cost->getRect()->moveTo(stat_upgrade->getRect()->right()+5, stat_upgrade->getRect()->y());
        cost_amount->getRect()->moveTo(cost->getRect()->right()+3, cost->getRect()->y());
    }
}

void Game::ToolTip::paint(QPainter *p){
    p->drawImage(*background->getRect(), *background->getImage());
    p->drawImage(*stat->getRect(), *stat->getImage());
    p->drawImage(*stat_upgrade->getRect(), *stat_upgrade->getImage());
    if(upgrade){
        p->drawImage(*cost->getRect(), *cost->getImage());
        p->drawImage(*cost_amount->getRect(), *cost_amount->getImage());
    }
}

void Game::ToolTip::resizeBackground(){
    int width = 2 + stat->getRect()->width() + 3 + stat_upgrade->getRect()->width();
    upgrade ? width += 5 + cost->getRect()->width() + 3 + cost_amount->getRect()->width() : width += 0;
    int height = stat->getRect()->height() + 4;
    background->setImage(background->getImage()->scaled(width, height, Qt::IgnoreAspectRatio));
    background->setRect(background->getImage()->rect());
}

