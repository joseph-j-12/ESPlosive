#ifndef HEADER_GColl
#define HEADER_GColl

#include "GComponent.h"

/*
Collider class. derived from GComponent. can be added as a component to an object by doing
auto* coll = CreateComponent<GColliderComp>(newObj, GColliderComp::ColliderType::Polygon, &shapetemplate1);

If the collider is not a circle a shape template must be assigned.
*/
 
class GObject;
struct ColliderShapeTemplate{
    int numPoints;
    Vec2D *points;
    ~ColliderShapeTemplate()
    {
        delete[] points;
    }
};

class GColliderComp : public GComponent{
    public:
    enum ColliderType{
        Circle,
        Polygon
    };

    GColliderComp(GObject* myParentObject, ColliderType col, float radius, Vec2D center = Vec2D(0,0));
    GColliderComp(GObject* myParentObject, ColliderType col, ColliderShapeTemplate *shape, Vec2D center = Vec2D(0,0));

    bool CalculateBoundingBox();
    void Begin() override;
    void Tick(float DeltaTime) override;

    bool trigger = false;
    bool notify_collision = false;
    
    ColliderShapeTemplate* myShape;
    float circleRadius;
    Vec2D myCenter;
    ColliderType myColliderType;

    Vec2D boundingBoxMax;
    Vec2D boundingBoxMin;

    Vec2D lastSeenBoundingBoxMin;
    Vec2D lastSeenBoundingBoxMax;
    
};
#endif