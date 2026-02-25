#ifndef CARSCENE
#define CARSCENE

#include <GScene.h>
#include "FinishLine.h"
#include "GInput.h"

// #include ""
class GObject;
class CarScene : public GScene{

    public :
    void Tick_Objects(float  DeltaTime);
    void Begin();
    void restart_level();

    // private:
    float terrain[128];// = {1000,35,15,15,15,45, 65, 10, 55, 25, 55, 12, 45, 12, 15, 100,10};
    float terrainPointSpacing = 100;
    int terrain_length = 45;
    float bottomYPos = -80;
    float initialOffset = -300;
    int terrainStart = 0;
    int terrainShownLength = 5;

    float difficulty = 0;

    // private:
    ColliderShapeTemplate groundPolygons[6];
    ColliderShapeTemplate playerBody;

    ColliderShapeTemplate FinishLineShape;

    ColliderShapeTemplate wheeltest;

    void setShapes();

    GObject* player;
    GObject* leftWheel;
    GObject* rightWheel;
    GObject* ground;

    FinishLineObject* finishLine;

    GInput_JoyStick* joy;
    GInput_Button* btn_L;
    GInput_Button* btn_U;

    float maxWheelVel = 15;
       
};
#endif