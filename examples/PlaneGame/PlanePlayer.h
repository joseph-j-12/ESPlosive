#ifndef PLANEPLAYER
#define PLANEPLAYER

#include <GScene.h>
#include "GInput.h"
#include <GColliderComp.h>
#include "PlaneScene.h"
#include "Arduino.h"
// Forward declaration so the player can talk to the scene
// class PlaneScene;

class PlanePlayer : public GObject {
public:
    PlaneScene* parentScene;

    PlanePlayer(GScene* scene) : GObject(scene) {
        my_id = 1; // Player ID
        parentScene = (PlaneScene*)scene;
        Serial.println("player spawn");
    }

    void RenderOnTFT(TFT_eSPI& tft, uint16_t outlineColor) override
    {
        GObject::RenderOnTFT(tft, TFT_GREEN);
    }
    void OnCollision(GPhysics::Collision col, GColliderComp* otherObj) override {
        if (parentScene->dead) return;
        if (otherObj->gobject->my_id == 5) { //hit obstacle
            parentScene->dead = true;

            otherObj->gobject->setPhysicsEnabled(true);
            otherObj->gobject->velocity = Vec2D(-parentScene->scrollSpeed,0);    

            parentScene->die();        
        }
    }
};

#endif