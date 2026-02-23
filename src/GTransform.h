#ifndef HEADER_GTransform
#define HEADER_GTransform

#include "Vec2D.h"
/*
Transform class - has a position and rotation. It can have a parent.
The parent transform will be applied on the object. for example 0,0 position will be the position of the parent

The position vector stores local position, rotation stores local rotation

*/
class GObject;
class GTransform{
    private:
    
    float CosandSin[4];

    float prevCosandSin[2];
    
    public:
    Vec2D position; //local position
    float rotation; //local rotation
    Vec2D scale;

    Vec2D prevPosition;
    float prevRotation;

    GTransform* parent;
    
    GObject *gobject;

    GTransform() : parent(nullptr), scale(1,1)
    {
        rotation = 0.f;
        prevRotation = 0.1f;
        calculateCosAndSine();
        calculatePrevCosAndSine();
    }

    void calculateCosAndSine()
    {
        if (prevRotation == rotation) return;
        prevRotation = rotation;
        CosandSin[0] = std::cos(rotation);
        CosandSin[1] = std::sin(rotation);
    }

    void calculatePrevCosAndSine()
    {
        //if (prevRotation == rotation) return;
        //prevRotation = rotation;
        prevCosandSin[0] = CosandSin[0];//std::cos(prevRotation);
        prevCosandSin[1] = CosandSin[1];//std::sin(prevRotation);
        prevPosition = position;
    }
    Vec2D myScenePosition()
    {
        if (parent == nullptr)
        {
            return position;
        } 
        calculateCosAndSine();
        return local_to_scene(Vec2D(0,0));
    }

    Vec2D local_to_scene(Vec2D pos)
    {
        if (parent == nullptr)
        {
            return position + Vec2D(pos.X*CosandSin[0] - pos.Y*CosandSin[1], pos.X*CosandSin[1] + pos.Y*CosandSin[0]);
        }
        else
        {
            return parent->local_to_scene(position + Vec2D(pos.X*CosandSin[0] - pos.Y*CosandSin[1], pos.X*CosandSin[1] + pos.Y*CosandSin[0]));
        }
    }

    Vec2D prev_local_to_scene(Vec2D pos)
    {
        if (parent == nullptr)
        {
            return prevPosition + Vec2D(pos.X*prevCosandSin[0] - pos.Y*prevCosandSin[1], pos.X*prevCosandSin[1] + pos.Y*prevCosandSin[0]);
        }
        else
        {
            return parent->prev_local_to_scene(prevPosition + Vec2D(pos.X*prevCosandSin[0] - pos.Y*prevCosandSin[1], pos.X*prevCosandSin[1] + pos.Y*prevCosandSin[0]));
        }
    }

    Vec2D local_to_scene_vector(Vec2D pos)
    {
        if (parent == nullptr)
        {
            return Vec2D(pos.X*CosandSin[0] - pos.Y*CosandSin[1], pos.X*CosandSin[1] + pos.Y*CosandSin[0]);
        }
        else
        {
            return parent->local_to_scene_vector(Vec2D(pos.X*CosandSin[0] - pos.Y*CosandSin[1], pos.X*CosandSin[1] + pos.Y*CosandSin[0]));
        }
    }

    Vec2D scene_to_local(Vec2D worldPos)
    {
        if (parent != nullptr)
        {
            worldPos = parent->scene_to_local(worldPos);
        }

        worldPos = worldPos - position;

        float c = CosandSin[0];
        float s = CosandSin[1];

        return Vec2D(worldPos.X * c + worldPos.Y * s, -worldPos.X*s + worldPos.Y*c);
    }

    Vec2D prev_scene_to_local(Vec2D worldPos)
    {
        if (parent != nullptr)
        {
            worldPos = parent->prev_scene_to_local(worldPos);
        }

        worldPos = worldPos - prevPosition;

        float c = prevCosandSin[0];
        float s = prevCosandSin[1];

        return Vec2D(worldPos.X * c + worldPos.Y * s, -worldPos.X*s + worldPos.Y*c);
    }

   
};

#endif