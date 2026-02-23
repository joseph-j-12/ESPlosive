#include "GScene.h"
#include "GCamera.h"
GScene::GScene() : physics(this), cam(this)
{

}

void GScene::Begin()
{
    for (auto& obj : sceneObjects)
    {
        obj->setInvMasses();
        if (!obj->enabled) continue;
        for (auto& comp : obj->myComponents)
        {
            comp->Begin();
        }        
    }
}


void GScene::Tick_Objects(float DeltaTime)
{
    cam.Tick(DeltaTime);
    for (auto& obj : sceneObjects)
    {
        obj->Tick(DeltaTime);
        for (auto& comp : obj->myComponents)
        {
            comp->Tick(DeltaTime);
        }        
    }
}

void GScene::Tick_Physics(float DeltaTime)
{
    //for (int i = 0; i < 5; i ++)
        physics.Tick(DeltaTime);
}

//todo
std::vector<std::unique_ptr<GObject>> GScene::GetChildrenOf(GObject *parentObj)
{
    std::vector<std::unique_ptr<GObject>> children;
    return children;
}
