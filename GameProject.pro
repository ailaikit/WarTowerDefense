QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GameProject
TEMPLATE = app


SOURCES += main.cpp\
        game.cpp \
    gameobject.cpp \
    enemy.cpp \
    button.cpp \
    tower.cpp \
    wavegenerator.cpp \
    image.cpp

HEADERS  += game.h \
    gameobject.h \
    enemy.h \
    waypoint.h \
    image.h \
    button.h \
    tile.h \
    tower.h \
    wavegenerator.h
