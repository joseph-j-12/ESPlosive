#include "GSpringComponent.h"
#include "GObject.h"
#include "GScene.h"
#include <algorithm>
#include <cmath>

// #include <iostream>

void GSpringComponent::Begin()
{
}

void GSpringComponent::Tick(float deltaTime)
{
    if (otherObject)
    {
        if (gobject)
        {
            if (!gobject->enabled || !otherObject->enabled) return;
            Vec2D myPointWorld = gobject->transform.local_to_scene(myAttachedPos);
            Vec2D otherPointWorld = otherObject->transform.local_to_scene(otherAttachedPos);

            Vec2D worldSpaceTargetDir = gobject->transform.local_to_scene_vector(attachDirection);
            Vec2D worldSpaceDir = myPointWorld-otherPointWorld;

            if (!constrainedAlongAxis)
            {
                float dist = mean_length - worldSpaceDir.magnitude();
                worldSpaceDir.normalize();

                gobject->myScene->physics.AddImpulseAtLocation(gobject, myPointWorld, worldSpaceDir*dist*spring_constant*deltaTime);
                gobject->myScene->physics.AddImpulseAtLocation(otherObject, myPointWorld, -worldSpaceDir*dist*spring_constant*deltaTime);
            }
            else
            {
                //constrain
                Vec2D normal = Vec2D::GetPerpendicular(worldSpaceTargetDir);

                normal.normalize(); 

                Vec2D r_me    = myPointWorld    - gobject->transform.myScenePosition();
                Vec2D r_other = otherPointWorld - otherObject->transform.myScenePosition();

                Vec2D vel_me    = gobject->velocity_at_point(myPointWorld);
                Vec2D vel_other = otherObject->velocity_at_point(otherPointWorld);

                Vec2D relVel = vel_me - vel_other;

                float Cdot = Vec2D::DotProduct(relVel, normal);

                float C = Vec2D::DotProduct(myPointWorld - otherPointWorld, normal);

                const float beta = 0.1f;
                float bias = beta * C / deltaTime;

                Cdot += bias;

                float rnA = Vec2D::CrossProduct(r_me, normal);
                float rnB = Vec2D::CrossProduct(r_other, normal);

                float invEffectiveMass =
                    (1.0f / gobject->mass) +
                    (1.0f / otherObject->mass) +
                    (rnA * rnA) / gobject->momentOfInertia +
                    (rnB * rnB) / otherObject->momentOfInertia;

                if (invEffectiveMass <= 0.0f) return;

                float lambda = -Cdot / invEffectiveMass;
                Vec2D impulse = normal * lambda;

                gobject->myScene->physics.AddImpulseAtLocation(
                  gobject, myPointWorld, impulse);

                gobject->myScene->physics.AddImpulseAtLocation(
                 otherObject, otherPointWorld, -impulse);
                


                //spring mechanics
                worldSpaceTargetDir.normalize();
                float projected = -Vec2D::DotProduct(worldSpaceDir, worldSpaceTargetDir);
                float dist = -(mean_length - (projected));
                worldSpaceDir.normalize();
                
                float force = dist*spring_constant;
                //force = std::clamp(force, -5000.f,5000.f);
                gobject->myScene->physics.AddImpulseAtLocation(gobject, myPointWorld, worldSpaceTargetDir*force*deltaTime);
                gobject->myScene->physics.AddImpulseAtLocation(otherObject, otherPointWorld, -worldSpaceTargetDir*force*deltaTime);

                
                //max and min length constrain
                // if (projected > maxLength)
                // {
                //     otherObject->transform.position = worldSpaceTargetDir*maxLength + myPointWorld;
                //     //std::cout << dist  << "  " << projected << std::endl;
                // }

                // if (projected < minLength)
                // {
                //     otherObject->transform.position = worldSpaceTargetDir*minLength + myPointWorld;
                //     //std::cout << dist  << "  " << projected << std::endl;
                // }


                //positional constrain
                const float slop = 0.001f;
                const float percent = 0.1f;
                float Cpos = Vec2D::DotProduct(myPointWorld - otherPointWorld, normal);

                // ignore very small errors
                if (fabs(Cpos) < slop)
                    return;

                float lambdaPos = -(percent * Cpos) / invEffectiveMass;

                Vec2D P = normal * lambdaPos;

                // linear
                gobject->transform.position += P * (1.0f / gobject->mass);
                otherObject->transform.position -= P * (1.0f / otherObject->mass);

                // // angular
                // float wA = Vec2D::CrossProduct(r_me, P) / gobject->momentOfInertia;
                // float wB = Vec2D::CrossProduct(r_other, P) / otherObject->momentOfInertia;

                // gobject->transform.rotation += wA;
                // otherObject->transform.rotation -= wB;

            }            
        }
    }
}
