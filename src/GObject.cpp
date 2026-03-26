#include "GObject.h"

void GObject::RenderOnTFT(TFT_eSPI& tft)
{
    if (!enabled) return;
    for (auto& coll : myComponents)
    {
        if (coll->_myCompType != GComponent::ComponentType::Collider) continue;

        //auto* col = static_cast<GColliderComp*>(coll.get());

        // if (!col) continue;;
        uint16_t outlineColor =
        getPhysicsEnabled() ? TFT_RED : TFT_BLUE;

        if (coll->renderComponent)
        {
            coll->Render(tft, outlineColor, this);
        }

    }
}

void GObject::ClearTFT(TFT_eSPI& tft, uint16_t color)
{
    if (!enabled) return;
    for (auto& coll : myComponents)
    {
        // if (coll->_myCompType != GComponent::ComponentType::Collider) continue;

        // auto* col = static_cast<GColliderComp*>(coll.get());

        // if (!col) continue;
        if (coll->renderComponent)
        {
            coll->Clear(tft,color);
        }
    }
}
