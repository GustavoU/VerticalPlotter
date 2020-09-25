#include <Arduino.h>

#define JOY_TYPE        1 //1 - Joystick físico ; 0 - Joystick HTML
#define MAP_JOYSTICK    1
#define BTN_PRESSED     LOW
#define BTN_NOT_PRESSED HIGH
#define SHORT_PRESS     100
#define MEDIUM_PRESS    500
#define LONG_PRESS      2000
#define SHORT_PRESSED   0
#define MEDIUM_PRESSED  2
#define LONG_PRESSED    4

typedef struct Boton {
    const int debounce = 2; 
    unsigned long counter=0;
    bool prevState = BTN_NOT_PRESSED;
    bool currentState;
    int action;
    bool done = false;
} Boton;

class Joystick
{
private:
    int jPinX;
    int jPinY;
    float offsetX = 0.0;
    float offsetY = 0.0;
    int X_Read;
    int Y_Read;
    int X_prev;
    int Y_prev;
    float xMap;
    float yMap;
    float xMapPrev;
    float yMapPrev;

    float angle = 0.0;
    float intensity = 0.0;
    int direction = 0;

public:
    Boton boton;

    Joystick(int PinX, int PinY)
    {
        jPinX = PinX;
        jPinY = PinY;
    };
    Joystick(int PinX, int PinY, int offset_X, int offset_Y)
    {
        jPinX = PinX;
        jPinY = PinY;
        offsetX = offset_X;
        offsetY = offset_Y;
    };

    int getValue_X();
    int getValue_Y();
    int getPrevValue_X();
    int getPrevValue_Y();
    float getMapped_X();
    float getMapped_Y();
    float getPrevMapped_X();
    float getPrevMapped_Y();

    float getAngle();
    float getDir();

};
