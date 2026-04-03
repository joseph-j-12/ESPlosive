#include "PlaneScene.h"
#include "PlanePlayer.h"


void PlaneScene::Begin() {
    // 1. Setup Player with the NEW class
    player = AddNewObject<PlanePlayer>();
    player->transform.position = Vec2D(50, 120);
    player->setPhysicsEnabled(true);
    
    playerBody.numPoints = 3;
    playerBody.points = new Vec2D[3]{ {15, 0}, {-10, 8}, {-10, -8} };
    
    // Create collider and enable notifications
    auto* pCol = player->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Polygon, &playerBody);
    pCol->notify_collision = true; 

    // 2. Setup Obstacles with ID 5
    obstacleShape.numPoints = 4;
    obstacleShape.points = new Vec2D[4]{ {-12,-12}, {12,-12}, {12,12}, {-12,12} };

    for (int i = 0; i < poolSize; i++) {
        obstaclePool[i] = AddNewObject<GObject>();
        obstaclePool[i]->my_id = 5; // Obstacle ID
        obstaclePool[i]->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Polygon, &obstacleShape);
        obstaclePool[i]->enabled = true;
        // obstaclePool[i]->setPhysicsEnabled(true);
    }

   horizontalWallShape.numPoints = 4;
    horizontalWallShape.points = new Vec2D[4]{
        {-160, -10}, {160, -10}, {160, 10}, {-160, 10}
    };

    // Vertical walls (20 units wide, 240 units tall)
    verticalWallShape.numPoints = 4;
    verticalWallShape.points = new Vec2D[4]{
        {-10, -120}, {10, -120}, {10, 120}, {-10, 120}
    };

    // --- 2. Spawn the 4 Wall Objects ---

    // Top Wall
    topWall = AddNewObject<GObject>();
    topWall->transform.position = Vec2D(160, 0); // Center-Top
    auto* wallcol = topWall->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Polygon, &horizontalWallShape);
    wallcol->renderComponent = false;
    topWall->setPhysicsEnabled(false); // Static wall
    topWall->my_id = 10;

    // Bottom Wall
    bottomWall = AddNewObject<GObject>();
    bottomWall->transform.position = Vec2D(160, 240); // Center-Bottom
    wallcol = bottomWall->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Polygon, &horizontalWallShape);
    wallcol->renderComponent = false;
    bottomWall->setPhysicsEnabled(false);
    bottomWall->my_id = 10;

    // Left Wall
    leftWall = AddNewObject<GObject>();
    leftWall->transform.position = Vec2D(0, 120); // Middle-Left
    wallcol = leftWall->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Polygon, &verticalWallShape);
    wallcol->renderComponent = false;
    leftWall->setPhysicsEnabled(false);
    leftWall->my_id = 10;

    // Right Wall
    rightWall = AddNewObject<GObject>();
    rightWall->transform.position = Vec2D(380, 120); // Middle-Right
    wallcol = rightWall->CreateComponent<GColliderComp>(GColliderComp::ColliderType::Polygon, &verticalWallShape);
    wallcol->renderComponent = false;
    rightWall->setPhysicsEnabled(false);
    rightWall->my_id = 10;


    //   cam.trackingTarget = player;
    cam.screenSize = Vec2D(320,240);
    cam.trackingSpeed = Vec2D(0.2f,0.2f);
    cam.position = Vec2D(150,120);
    cam.maxTrackDistance = 100;

    physics.gravity = 0;

    GScene::Begin();

    for (int i = 0; i < poolSize; i++) {
        
        obstaclePool[i]->enabled = false;
    }
}

void PlaneScene::Tick_Objects(float DeltaTime) {
    GScene::Tick_Objects(DeltaTime);

    // Movement logic (using velocity for physics compatibility)
    if (!dead)
    {
        if (joy) {
            joy->Tick();
            player->velocity.X = joy->horizontal * 80.0f;
            player->velocity.Y = joy->vertical * 80.0f;
            player->transform.rotation = joy->vertical;
        }

        // Spawn/Recycle Logic
        spawnTimer += DeltaTime;
        if (spawnTimer >= spawnRate) {
            SpawnObstacle();
            spawnTimer = 0;
        }

        for (int i = 0; i < poolSize; i++) {
            if (!obstaclePool[i]->enabled) continue;
            obstaclePool[i]->transform.position.X -= scrollSpeed * DeltaTime;
            // Serial.println(obstaclePool[i]->transform.position.X);
            if (obstaclePool[i]->transform.position.X < -50) 
            {
                // Serial.println(obstaclePool[i]->transform.position.X);
                obstaclePool[i]->enabled = false;
                obstaclePool[i]->velocity = Vec2D();
            }
        }
    }
}

void PlaneScene::SpawnObstacle() {
    for (int i = 0; i < poolSize; i++) {
        if (!obstaclePool[i]->enabled) {
            obstaclePool[i]->enabled = true;
            obstaclePool[i]->transform.position = Vec2D(350, random(20, 220)); 
            return;
        }
    }
}

void PlaneScene::die()
{
    for (int i = 0; i < poolSize; i++) {
        if (obstaclePool[i]->enabled) {
            obstaclePool[i]->setPhysicsEnabled(true);
            obstaclePool[i]->velocity = Vec2D(-scrollSpeed,0);    
        }
    }
    
    
}
void PlaneScene::restart_level() {
    dead = false;
    player->transform.position = Vec2D(50, 120);
    player->velocity = Vec2D(0, 0);
    player->angularVelocity = 0.f;
    player->transform.rotation = 0.f;
    for (int i = 0; i < poolSize; i++) {
        obstaclePool[i]->enabled = false;
        obstaclePool[i]->setPhysicsEnabled(false);
        obstaclePool[i]->transform.rotation = 0.0f;
    }
}