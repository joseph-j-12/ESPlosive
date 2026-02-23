#ifndef HEADER_GPhysics
#define HEADER_GPhysics
#include "Vec2D.h"
#include "GTransform.h"
//#include "SFML/Graphics.hpp"
#include "GSpatialHash.h"

class GScene;
class GColliderComp;
//Physics class. Handles the collision detection and collision resolution of objects;
//The physics enabled object should not be a child object. if it is a child object, 
//its parent must be at origin and have zero rotation.
class GPhysics
{
public:
    struct Collision{
        Vec2D point;
        Vec2D normal; //in the direction to push col2 out (col1 to col2)
        float depth;
        GColliderComp* col1; //col1 is the object whose side is the intersecting axis
        GColliderComp* col2; //col2 is the object whose point is inside the other object

        bool colliding;
    };

    enum ForceType{
        Force,
        Impulse
    };

    GPhysics(GScene* scene);
    void Tick(float DeltaTime);
    void Begin();

    Collision GetCollisionBetweenObjects(GColliderComp* obj1, GColliderComp* obj2);
    void HandleCollision(Collision col);

    void AddImpulseAtLocation(GObject* const object, Vec2D location , Vec2D force);
    void AddVelocityAtLocation(GObject* const object, Vec2D location , Vec2D velocity);

    GColliderComp* gColliders[MAX_COLLIDERS];
    uint16_t gColliderCount = 0;

    uint16_t registerCollider(GColliderComp* col);

    GSpatialHash grid;
    private:
    
    GScene* myScene;
    void ApplyVelocities(float DeltaTime);
    void Damping(float DeltaTime);

    void UpdateSpatialHashGrid();
    void BroadPhase();
    

};
#endif