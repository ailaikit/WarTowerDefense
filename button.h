#ifndef BUTTON_H
#define BUTTON_H

#include "image.h"


class Button : public Image
{
public:

    Button(QString filePath, QString h_filePath, qreal scale);
    Button(Image* passive, Image* active);
    ~Button();


      void setActive(bool a){ active = a; }


      bool isActive() const { return active; }


      QImage getActiveImage() const { return *activeImage; }
private:

    QImage* activeImage;
    bool active;
};

#endif
