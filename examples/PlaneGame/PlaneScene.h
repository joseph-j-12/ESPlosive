#ifndef PLANESCENE
#define PLANESCENE

#include <GScene.h>
#include "GInput.h"
#include <GColliderComp.h>
// #include "PlanePlayer.h"
class PlanePlayer;
class PlaneScene : public GScene {
public:
    void Tick_Objects(float DeltaTime) override;
    void Begin() override;
    void restart_level();
    void SpawnObstacle();
    void die();
    ColliderShapeTemplate playerBody;
    ColliderShapeTemplate obstacleShape;

    PlanePlayer* player; // Using the new custom class
    GObject* obstaclePool[10]; //pooling obstacles
    int poolSize = 10;

    GInput_JoyStick* joy;
    float spawnTimer = 0;
    float spawnRate = 2.2f;
    float scrollSpeed = 50.0f;


    ColliderShapeTemplate horizontalWallShape;
    ColliderShapeTemplate verticalWallShape;

    // Wall Objects
    GObject* topWall;
    GObject* bottomWall;
    GObject* leftWall;
    GObject* rightWall;

    bool dead = false;
};


#endif