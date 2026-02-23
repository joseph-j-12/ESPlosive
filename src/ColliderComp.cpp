#include "ColliderComp.h"
#include "GObject.h"
#include "GScene.h"
#include "GPhysics.h"
// #include <iostream>
GColliderComp::GColliderComp(GObject *myParentObject, ColliderType col, float radius, Vec2D center) : GComponent(myParentObject)
{
    myColliderType = col;
    circleRadius = radius;
    myCenter = center;
    _myCompType = GComponent::ComponentType::Collider;
}

GColliderComp::GColliderComp(GObject *myParentObject, ColliderType col, ColliderShapeTemplate *shape, Vec2D center) : GComponent(myParentObject)
{
    myShape = shape;
    myColliderType = col;
    myCenter = center;
    _myCompType = GComponent::ComponentType::Collider;
}

bool GColliderComp::CalculateBoundingBox()
{
    //std::cout << "bound" << std::endl;
    if (gobject == nullptr){ return false;}
    gobject->transform.calculateCosAndSine();
    
    if (myColliderType == ColliderType::Polygon)
    {
        Vec2D pointTransformed = myShape->points[0] + myCenter;
        float maxX = gobject->transform.local_to_scene(pointTransformed).X; 
        float minX = gobject->transform.local_to_scene(pointTransformed).X; 
        float maxY = gobject->transform.local_to_scene(pointTransformed).Y; 
        float minY = gobject->transform.local_to_scene(pointTransformed).Y;
        for (int i = 0; i < myShape->numPoints; i++)
        {
            Vec2D transformedPoint = gobject->transform.local_to_scene(myShape->points[i] + myCenter);
            if (transformedPoint.X > maxX) maxX = transformedPoint.X;
            if (transformedPoint.Y > maxY) maxY = transformedPoint.Y;
            if (transformedPoint.X < minX) minX = transformedPoint.X;
            if (transformedPoint.Y < minY) minY = transformedPoint.Y;
            
        }
        boundingBoxMax = Vec2D(maxX, maxY);
        boundingBoxMin = Vec2D(minX, minY);
        return true;
    }
    if (myColliderType == ColliderType::Circle)
    {
        Vec2D centerTransformed = gobject->transform.local_to_scene(myCenter);
        boundingBoxMax = Vec2D(centerTransformed.X + circleRadius, centerTransformed.Y + circleRadius);
        boundingBoxMin = Vec2D(centerTransformed.X - circleRadius, centerTransformed.Y - circleRadius);
        return true;
    }
    
    return false;
    
}

void GColliderComp::Begin()
{
    gobject->myScene->physics.registerCollider(this);
}

void GColliderComp::Tick(float DeltaTime)
{
    //gobject->myScene->physics.grid.InsertCollider(this);
}
