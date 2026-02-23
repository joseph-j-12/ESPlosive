#ifndef HEADER_GCAMERA
#define HEADER_GCAMERA

#include "Vec2D.h"
#include "GObject.h"
// class GObject;
/*
 C amera* class
 Manages the scroll values of the camera which is to be subtracted from coordinates
 of each point before converting to screen coordinates.

 It also supports object tracking. If not assigned (nullptr) it wont track anything
 */
class GCamera{
public :
    Vec2D position;
    float zoom;

    Vec2D screenSize;

    GObject* trackingTarget;
    Vec2D trackingSpeed;
    float maxTrackDistance = INFINITY;
    GScene* scene;

    GCamera(GScene* myscene)
    {
        zoom = 1;
        shake = Vec2D(0,0);
        scene = myscene;
    }
    void Tick(float deltaTime)
    {
        inverseZoom = 1/zoom;
        if (trackingTarget)
        {
            if (trackingSpeed.X <= 0.001f && trackingSpeed.Y <= 0.001f)
            {
                position = trackingTarget->transform.myScenePosition();
            }
            else
            {
                position += (trackingTarget->transform.myScenePosition() - position)*trackingSpeed*deltaTime;
            }
            Vec2D dir = position - trackingTarget->transform.position;
            float dist = dir.magnitude();

            if (dist > maxTrackDistance)
            {
                position = trackingTarget->transform.position + dir.getNormal()*maxTrackDistance;
            }
        }
    }

    Vec2D convertScreenToWorld(Vec2D screenPos)
    {
        return Vec2D((screenPos.X-screenSize.X*0.5f)*inverseZoom + position.X,-(screenPos.Y-screenSize.Y*0.5f)*inverseZoom + position.Y);
    }

    Vec2D convertWorldToScreen(Vec2D worldPos)
    {
        return Vec2D(zoom*(worldPos.X-position.X)+screenSize.X*0.5f, zoom*(-worldPos.Y+position.Y)+screenSize.Y*0.5f);
    }

    Vec2D convertWorldToScreenRender(Vec2D worldPos)
    {
        return Vec2D(zoom*(worldPos.X-position.X)+screenSize.X*0.5f, zoom*(-worldPos.Y+position.Y)+screenSize.Y*0.5f);
    }
    float inverseZoom;
private:
    Vec2D shake;
};
#endif
