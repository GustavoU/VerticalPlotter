#include "Joystick.h"

int Joystick::getValue_X()
{
    X_prev = X_Read;
    X_Read = analogRead(jPinX);
    return X_Read;
}
int Joystick::getValue_Y()
{
    Y_prev = Y_Read;
    Y_Read = analogRead(jPinY);
    return Y_Read;
}
float Joystick::getMapped_X()
{
    xMapPrev = xMap;
    X_Read = analogRead(jPinX);
    if (X_Read > offsetX)
    {
        xMap = ((X_Read - offsetX) / (4095.0 - offsetX));
    }
    else
    {
        xMap = ((X_Read - offsetX) / (offsetX));
    }
    //xMap = map(X_Read, 0,4095, -1, 1);
    return xMap;
}
float Joystick::getMapped_Y()
{
    yMapPrev = yMap;
    Y_Read = analogRead(jPinY);
    if (Y_Read > offsetY)
    {
        yMap = ((Y_Read - offsetY) / (4095.0 - offsetY));
    }
    else
    {
        yMap = ((Y_Read - offsetY) / (offsetY));
    }
    //yMap = map(Y_Read, 0,4095, -1, 1);
    return yMap;
}

float Joystick::getAngle(){
    angle = atan(yMap/xMap);
    return angle;
}

int Joystick::getPrevValue_X()
{
    return X_prev;
}
int Joystick::getPrevValue_Y()
{
    return Y_prev;
}
float Joystick::getPrevMapped_X()
{
    return xMapPrev;
}
float Joystick::getPrevMapped_Y()
{
    return yMapPrev;
}