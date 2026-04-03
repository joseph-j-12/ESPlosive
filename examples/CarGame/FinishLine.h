#ifndef FINISHLINE
#define FINISHLINE

#include <GObject.h>

class FinishLineObject : public GObject
{
  public :
  bool player_reached = false;
  FinishLineObject(GScene* scene) : GObject(scene) 
  {
    my_id = 15;
  }
  void OnCollision(GPhysics::Collision col, GColliderComp* otherObj) override {
    if (otherObj->gobject->my_id == 1)
    player_reached = true;
    // colliding = fals;
  }

  void RenderOnTFT(TFT_eSPI& tft, uint16_t outlineColor) override
  {
    GObject::RenderOnTFT(tft, player_reached ? TFT_YELLOW : TFT_GREEN);
  }
  private:
  bool colliding;
};

#endif