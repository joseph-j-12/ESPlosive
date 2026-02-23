#ifndef HEADER_GCOMP
#define HEADER_GCOMP

#include "Vec2D.h"
#include "GTransform.h"

class GObject;
/*
A Component is attached to GObject. Multiple components can be attached.
The Begin function will be called at start after all the constructors everywhere has initialized
Tick is called every frame if enableComponentTick is true
Different types of components can be created from this component class
*/

class GComponent{
    public:
    bool enableComponentTick;

    GComponent(GObject *myParentObject)
    {
        gobject = myParentObject;
    }
    
    enum ComponentType{
        Collider,
        Spring
    };

    virtual ~GComponent() = default;

    virtual void Begin(){};
    virtual void Tick(float deltaTime){};

    void onEnabled(){};

    void setEnabled(bool enabled)
    {
        componentEnabled = enabled;
        if (enabled)
        {
            onEnabled();
        }
    }
    
    GObject *gobject;
    ComponentType _myCompType;
    private:
    bool componentEnabled;
    

};
#endif