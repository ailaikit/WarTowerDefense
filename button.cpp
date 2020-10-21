#include "button.h"
#include <QImage>
}

Button::Button(Image* passive, Image* active) : Image(*passive), activeImage(active->getImage()), active(false){}

Button::~Button(){
    delete activeImage;
}
