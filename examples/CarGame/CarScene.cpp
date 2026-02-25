#include "CarScene.h"
#include <GSpringComponent.h>
#include <GColliderComp.h>
#include "FinishLine.h"
//#include ""
// #include <iostream>

void CarScene::Tick_Objects(float DeltaTime)
{
    GScene::Tick_Objects(DeltaTime);
    setShapes();
    terrainStart = round((cam.position.X+100)/terrainPointSpacing);
    if (joy)
    {
        joy->Tick();
        float y = -joy->horizontal;
        if (abs(y) >= 0.001){
        leftWheel->angularVelocity += 250*abs(y)*0.015 * (maxWheelVel*y - leftWheel->angularVelocity)/(maxWheelVel*abs(y));
        rightWheel->angularVelocity += 250*abs(y)*0.015 * (maxWheelVel*y - rightWheel->angularVelocity)/(maxWheelVel*abs(y));
        }
    }
}

void CarScene::Begin()
{   
    ground = AddNewObject<GObject>();
    for (int i = 0; i < terrainShownLength; i++)
    {
        groundPolygons[i].points = new Vec2D[4];
        groundPolygons[i].numPoints = 4;
        ground->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Polygon, &groundPolygons[i]);
    }
    ground->friction = 0.9f;
    ground->bounce = 0.2f;

    int lastHeight = 15;
    for (int i = 0; i < terrain_length-2; i++)
    {
        if (i < 6) {
        terrain[i] = lastHeight;
        continue;
        }
        Serial.println(lastHeight);
        int min = -(60 + difficulty*15);
        int max = (60 + difficulty*15);
        if (lastHeight < 0) min = 0;
        if (lastHeight > 300) max = 0;
        terrain[i] = (lastHeight + round(random(min,max)));//*(2*std::sin(lastHeight*i/25)+1);
        lastHeight = terrain[i];
        
    }
    terrain[terrain_length-2] = lastHeight;
    terrain[terrain_length-1] = lastHeight+160;
    terrain[0] = terrain[0] + 200;

    finishLine = AddNewObject<FinishLineObject>();
    finishLine->transform.position = Vec2D(initialOffset + (terrain_length-2.5f)*terrainPointSpacing, lastHeight+25);
    FinishLineShape.numPoints = 4;
    FinishLineShape.points = new Vec2D[4];
    FinishLineShape.points[0].X = -25;
    FinishLineShape.points[0].Y = -25;

    FinishLineShape.points[1].X = -25;
    FinishLineShape.points[1].Y = 25;

    FinishLineShape.points[2].X = 25;
    FinishLineShape.points[2].Y = 25;

    FinishLineShape.points[3].X = 25;
    FinishLineShape.points[3].Y = -25;

    auto* finishCol = finishLine->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Polygon, &FinishLineShape);
    finishCol->trigger = true;
    finishCol->notify_collision = true;

    playerBody.numPoints = 8;
    playerBody.points = new Vec2D[8];
    playerBody.points[0].X = -35;
    playerBody.points[0].Y = -8;

    playerBody.points[1].X = -35;
    playerBody.points[1].Y = 8;

    playerBody.points[2].X = -15;
    playerBody.points[2].Y = 8;
    
    playerBody.points[3].X = -15;
    playerBody.points[3].Y = 15;

    playerBody.points[4].X = 15;
    playerBody.points[4].Y = 15;

    playerBody.points[5].X = 15;
    playerBody.points[5].Y = 8;

    playerBody.points[6].X = 35;
    playerBody.points[6].Y = 8;

    playerBody.points[7].X = 35;
    playerBody.points[7].Y = -8;

    rightWheel = AddNewObject<GObject>();
    auto* coll4 = rightWheel->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Circle, 8);
    rightWheel->setPhysicsEnabled(true);
    rightWheel->transform.position = Vec2D(25, 150);
    rightWheel->mass = 25;
    rightWheel->my_id = 1;
    rightWheel->transform.rotation = 1.5f;
    rightWheel->momentOfInertia = 5000;
    rightWheel->bounce = 0.1f;
    rightWheel->friction = 0.8f;
    rightWheel->angularVelocity = -5.f;

    leftWheel = AddNewObject<GObject>();
    auto* coll = leftWheel->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Circle, 8);
    leftWheel->setPhysicsEnabled(true);
    leftWheel->transform.position = Vec2D(-25, 150);
    leftWheel->mass = 25;
    leftWheel->my_id = 1;
    leftWheel->transform.rotation = 1.5f;
    leftWheel->momentOfInertia = 5000;
    leftWheel->bounce = 0.1f;
    leftWheel->friction = 0.8f;

    player =  AddNewObject<GObject>();
    coll = player->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Polygon, &playerBody, Vec2D(0,0));
    coll = player->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Circle, 8, Vec2D(35,0));
    coll = player->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Circle, 8, Vec2D(-35,0));
    player->setPhysicsEnabled(true);
    player->transform.position = Vec2D(0, 200);
    player->my_id = 1;
    auto* spring = player->CreateComponent<GSpringComponent>(leftWheel);
    spring->spring_constant = 3000.f;
    spring->constrainedAlongAxis = true;
    spring->myAttachedPos = Vec2D(-25,0);
    spring->attachDirection = Vec2D(0,-1);
    spring->mean_length = 30.f;
    spring->minLength = 14;
    spring->maxLength = 155;

    auto* spring2 = player->CreateComponent<GSpringComponent>(rightWheel);
    spring2->spring_constant = 3000.f;
    spring2->constrainedAlongAxis = true;
    spring2->myAttachedPos = Vec2D(25,0);
    //spring->otherAttachedPos = Vec2D(0,25);
    spring2->attachDirection = Vec2D(0,-1);
    spring2->mean_length = 30.f;
    spring2->minLength = 14;
    spring2->maxLength = 155;

    //newObj->velocity = Vec2D(0,-100);
    player->friction = 0.3f;
    player->mass = 1000;
    player->momentOfInertia = 2000000;
    player->bounce = 0.2f;
    
    cam.trackingTarget = player;
    cam.screenSize = Vec2D(320,240);
    cam.trackingSpeed = Vec2D(0.2f,0.2f);
    cam.position = player->transform.position;
    cam.maxTrackDistance = 100;

    GScene::Begin();
}

void CarScene::restart_level()
{
    player->transform.position = Vec2D(0, 200);
    player->velocity = Vec2D();
    player->angularVelocity = 0.f;
    player->transform.rotation = 0.f;

    rightWheel->transform.position = Vec2D(25, 150);
    rightWheel->velocity = Vec2D();
    rightWheel->angularVelocity = 0.f;
    rightWheel->transform.rotation = 0.f;

    leftWheel->transform.position = Vec2D(-25, 150);
    leftWheel->velocity = Vec2D();
    leftWheel->angularVelocity = 0.f;
    leftWheel->transform.rotation = 0.f;

    finishLine->player_reached = false;


}

void CarScene::setShapes()
{
    if (terrainStart < 0) terrainStart = 0;
    if (terrainStart > terrain_length - terrainShownLength)
    terrainStart = terrain_length - terrainShownLength;

    int terrainShowend = terrainStart + terrainShownLength;


    for (int i = terrainStart; i < terrainShowend-1; i++)
    {
        int j = i - terrainStart;
        groundPolygons[j].points[3] = Vec2D(initialOffset+i*terrainPointSpacing,terrain[i]-25);
        groundPolygons[j].points[2] = Vec2D(initialOffset+(i+1)*terrainPointSpacing,terrain[i+1]-25);
        groundPolygons[j].points[1] = Vec2D(initialOffset+(i+1)*terrainPointSpacing,terrain[i+1]);
        groundPolygons[j].points[0] = Vec2D(initialOffset+i*terrainPointSpacing,terrain[i]);
    }
}
