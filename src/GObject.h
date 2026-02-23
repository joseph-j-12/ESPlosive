#ifndef HEADER_GOBJ
#define HEADER_GOBJ

#include "Vec2D.h"
#include "GTransform.h"
#include "GPhysics.h"
#include "GComponent.h"
#include <vector>
#include <memory>
#include "ColliderComp.h"

// #include <stdio.h>
// #include <iostream>
/*
 An object that will exist in a scene
 Every object will have a transform and the physicsEnabled flag.
 */
class GScene;
class GObject{
public:
    GTransform transform;
    Vec2D velocity; //world space velocity
    float angularVelocity;
    int my_id = 0;
    GScene* myScene;
    //physics collision paramters
    float mass;
    float momentOfInertia;
    float bounce; //coefficient of restitution
    float friction;

    bool enabled;

    float invMass;
    float invMomentOfInertia;

    GObject(GScene* scene)
    {
        physicsEnabled = false;
        transform.gobject = this;
        angularVelocity = 0.f;
        mass = 30;
        momentOfInertia = 3000;
        bounce = 0.1f;
        friction = 0.1f;
        myScene = scene;
        enabled = true;
        setInvMasses();

    }

    //to be called every time mass or moment of inertia changes
    void setInvMasses()
    {
        invMass = 1/mass;
        invMomentOfInertia = 1/momentOfInertia;
    }
    virtual void Begin() {setInvMasses();};

    virtual void Tick(float DeltaTime) {};

    virtual void OnCollision(GPhysics::Collision col, GColliderComp* otherObj) { };

    virtual ~GObject() = default;

    template<typename T, typename... Args> T* CreateComponent(Args&&... args)
    {
        auto newComp = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* ptr = newComp.get();
        myComponents.push_back(std::move(newComp));

        return ptr;
    }

    bool getPhysicsEnabled() {return physicsEnabled;}
    void setPhysicsEnabled(bool en) {physicsEnabled = en;}

    std::vector<std::unique_ptr<GComponent>> myComponents;

    Vec2D velocity_at_point(Vec2D worldPos)
    {
        Vec2D vel = velocity;
        Vec2D dir = worldPos - transform.local_to_scene(Vec2D(0,0));
        //float length = dir.magnitude();

        Vec2D angular = Vec2D::CrossProductZWithVector(angularVelocity, dir);

        return (vel+angular);
    }

private:
    bool physicsEnabled;
    bool colliderEnabled;
};
#endif
