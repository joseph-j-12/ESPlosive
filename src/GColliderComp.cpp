#include "GColliderComp.h"
#include "GObject.h"
#include "GScene.h"
#include "GPhysics.h"
// #include <iostream>]


void GColliderComp::Render(TFT_eSPI& tft, uint16_t outlineColor, GObject* obj)
{
    GCamera* cam = &obj->myScene->cam;
    if (myColliderType == GColliderComp::ColliderType::Polygon)
    {
        int n = myShape->numPoints;
        static Vec2D screenPts[16]; // adjust max if needed
        for (int i = 0; i < n; i++)
        {
            Vec2D p =
            obj->transform.local_to_scene(
                myShape->points[i]
            );

            screenPts[i] = cam->convertWorldToScreen(p);
        }

        // Draw polygon outline
        // drawPolygon(tft, screenPts, n, outlineColor);
        for (int i = 0; i < n; i++)
        {
            int j = (i + 1) % n;
            tft.drawLine(
                screenPts[i].X, screenPts[i].Y,
                screenPts[j].X, screenPts[j].Y,
                outlineColor
            );
        }

        CalculateBoundingBox();
        lastSeenBoundingBoxMin = cam->convertWorldToScreen(boundingBoxMin);
        lastSeenBoundingBoxMax = cam->convertWorldToScreen(boundingBoxMax);
        // Vec2D bTop =
        // cam->convertWorldToScreen(
        //     Vec2D(boundingBoxMin.X, boundingBoxMax.Y)
        // );

        // Vec2D bBottom =
        // cam->convertWorldToScreen(
        //     Vec2D(boundingBoxMax.X, boundingBoxMin.Y)
        // );

        // int w = bBottom.X - bTop.X;
        // int h = bBottom.Y - bTop.Y;

        // Uncomment if you want bounding boxes
        //tft.drawRect(bTop.X, bTop.Y, w, h, TFT_GREEN);
    }


    else if (myColliderType == GColliderComp::ColliderType::Circle)
    {
        Vec2D centerWorld =
        obj->transform.local_to_scene(myCenter);

        Vec2D center =
        cam->convertWorldToScreen(centerWorld);

        int r = (int)circleRadius;

        CalculateBoundingBox();
        lastSeenBoundingBoxMin = cam->convertWorldToScreen(boundingBoxMin);
        lastSeenBoundingBoxMax = cam->convertWorldToScreen(boundingBoxMax);

        tft.drawCircle(center.X, center.Y, r, outlineColor);

        // drawCircleWithAxis(
        //     tft,
        //     center.X,
        //     center.Y,
        //     r,
        //     outlineColor
        // );
    }
}

void GColliderComp::Clear(TFT_eSPI& tft, uint16_t color)
{
    Vec2D p1 = lastSeenBoundingBoxMin;
            Vec2D p2 = lastSeenBoundingBoxMax;
            // Serial.println(p2.Y-p1.Y);
            tft.fillRect(
                p1.X-3, 
                p2.Y-3, 
                p2.X-p1.X+6, 
                p1.Y-p2.Y+6, 
                TFT_BLACK
            );
}

GColliderComp::GColliderComp(GObject *myParentObject, ColliderType col, float radius, Vec2D center) : GComponent(myParentObject)
{
    myColliderType = col;
    circleRadius = radius;
    myCenter = center;
    _myCompType = GComponent::ComponentType::Collider;
    renderComponent = true;
    clearComponent = true;
}

GColliderComp::GColliderComp(GObject *myParentObject, ColliderType col, ColliderShapeTemplate *shape, Vec2D center) : GComponent(myParentObject)
{
    myShape = shape;
    myColliderType = col;
    myCenter = center;
    _myCompType = GComponent::ComponentType::Collider;
    renderComponent = true;
    clearComponent = true;
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
