#ifndef HEADER_GScene
#define HEADER_GScene

#include "Vec2D.h"
#include <vector>
#include "GObject.h"
#include <memory>
#include "GPhysics.h"
#include "Arduino.h"
/*
Scene contains all the objects. It runs the tick for all the objects and components. 
It connects rigidbodies and physics and also handles searching for children of an object
*/
class GScene{
    public :
    std::vector<std::unique_ptr<GObject>> sceneObjects;

    GScene();

    void Tick_Objects(float DeltaTime);
    void Tick_Physics(float DeltaTime);
    void Begin();

    GPhysics physics;

    std::vector<std::unique_ptr<GObject>> GetChildrenOf(GObject* parentObj);

    template<typename T, typename... Args> T* AddNewObject(Args&&... args)
    {
        auto newObj = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* ptr = newObj.get();
        sceneObjects.push_back(std::move(newObj));
        return ptr;
    }
};

#endif