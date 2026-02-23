#include "GPhysics.h"
#include "GComponent.h"
#include "GScene.h"
#include "GObject.h"
#include "GColliderComp.h"
#include "GSpatialHash.h"
#include <iostream>
#include <cmath>
//#include <array>
#include <algorithm>


GPhysics::GPhysics(GScene* scene) : grid()
{
    myScene = scene;
}

void GPhysics::Tick(float DeltaTime)
{

    ApplyVelocities(DeltaTime);
    for (auto& obj : myScene->sceneObjects)
    {
        if (!obj->getPhysicsEnabled() || !obj->enabled) continue;
        obj->velocity = obj->velocity + Vec2D(0,-15.8)*DeltaTime;
        // obj->setInvMasses();
    }
    Damping(DeltaTime);
    //std::cout << "test1" << std::endl;
    UpdateSpatialHashGrid();
    //std::cout << "test2" << std::endl;
    BroadPhase();

}


void GPhysics::Begin()
{

}

void GPhysics::UpdateSpatialHashGrid()
{
    grid.Clear();
    for (int i = 0; i < gColliderCount; i++)
    {
        //uint16_t a = 8;
        if (gColliders[i]->gobject->enabled)
            grid.InsertCollider(gColliders[i], i);
        //std::cout << i << std::endl;
    }
}

void GPhysics::BroadPhase()
{
    for (int b = 0; b < MAX_BUCKETS; b++)
    {
        for (int i = grid.bucketHead[b]; i != -1; i = grid.entries[i].next)
        {
            for (int j = grid.entries[i].next; j != -1; j = grid.entries[j].next)
            {
                //std::cout << j << std::endl;
                uint16_t A = grid.entries[i].colliderIndex;
                uint16_t B = grid.entries[j].colliderIndex;

                //std::cout << A << "-" << B << std::endl;
                if (A != B)
                {
                    Collision col = GetCollisionBetweenObjects(gColliders[A], gColliders[B]);

                    if (col.colliding)
                    {
                        if (!col.col1->trigger && !col.col2->trigger)
                            HandleCollision(col);

                        if (col.col1->notify_collision)
                            col.col1->gobject->OnCollision(col, col.col2);
                        if (col.col2->notify_collision)
                            col.col2->gobject->OnCollision(col, col.col1);
                    }
                    //GetColl(gColliders[A], gColliders[B]);
                }
            }
        }
    }
}

//return the collision between two objects.
GPhysics::Collision GPhysics::GetCollisionBetweenObjects(GColliderComp *obj1, GColliderComp *obj2)
{
    //Separating axis theorem
    GColliderComp* o1 = obj1;
    GColliderComp* o2 = obj2;
    GColliderComp* object_ther_than_axis;
    GColliderComp* object_that_is_axis;
    o1->gobject->transform.calculateCosAndSine();
    o2->gobject->transform.calculateCosAndSine();
    float minOverlap = INFINITY;
    Vec2D overlapNormal;
    Vec2D point;
    Collision col;
    for (int i = 0; i < 2; i++)
    {
        if (i == 1)
        {
            o1 = obj2;
            o2 = obj1;
        }

        if (o1->myColliderType == GColliderComp::ColliderType::Polygon)
        {
            Vec2D center1;
            Vec2D center2;
            for (int j = 0; j < o1->myShape->numPoints; j++)
            {
                int k = (j+1);
                if (k>=o1->myShape->numPoints) k = 0;

                Vec2D axis = Vec2D::GetPerpendicular(o1->gobject->transform.local_to_scene(o1->myShape->points[k]+o1->myCenter)-o1->gobject->transform.local_to_scene(o1->myShape->points[j]+o1->myCenter)).getNormal();

                //finding projection of 1st shape
                float min_r1 = INFINITY; float max_r1 = -INFINITY;
                center1 = Vec2D(0,0);
                center2 = Vec2D(0,0);
                for (int p = 0; p < o1->myShape->numPoints; p++)
                {
                    Vec2D pt = o1->gobject->transform.local_to_scene(o1->myShape->points[p]+o1->myCenter);
                    float proj = Vec2D::DotProduct(axis, pt);
                    min_r1 = std::min(min_r1, proj);
                    max_r1 = std::max(max_r1, proj);
                    center1 = center1 + pt;
                }
                center1 = center1/o1->myShape->numPoints;

                //2nd shape
                float min_r2 = INFINITY; float max_r2 = -INFINITY;
                if (o2->myColliderType == GColliderComp::Polygon)
                {
                    for (int p = 0; p < o2->myShape->numPoints; p++)
                    {
                        Vec2D pt = o2->gobject->transform.local_to_scene(o2->myShape->points[p]+o2->myCenter);
                        float proj = Vec2D::DotProduct(axis, pt);
                        min_r2 = std::min(min_r2, proj);
                        max_r2 = std::max(max_r2, proj);
                        center2 = center2 + pt;
                    }
                    center2 = center2/o2->myShape->numPoints;
                }

                //if one of the colliders is a circle
                else if (o2->myColliderType == GColliderComp::Circle)
                {
                    //Vec2D side1 = o2->gobject->transform.myScenePosition() - axis*o2->circleRadius;
                    //Vec2D side2 = o2->gobject->transform.myScenePosition() + axis*o2->circleRadius;

                    //float proj1 = Vec2D::DotProduct(axis, side1);
                    //float proj2 = Vec2D::DotProduct(axis, side2);
                    center2 = o2->gobject->transform.local_to_scene(o2->myCenter);
                    float centerproj = Vec2D::DotProduct(axis, center2);

                    min_r2 = centerproj - o2->circleRadius;//std::min(min_r2, std::min(proj1,proj2));
                    max_r2 = centerproj + o2->circleRadius;//std::max(max_r2, std::max(proj1,proj2));
                }

                //if not overlaping on any one axis, then the objects are not colliding
                if (!(max_r2 >= min_r1 && max_r1 >= min_r2)) {
                    col.depth = -1;
                    col.colliding = false;
                    return col;
                }

                //calculate overlap and normal of overlap
                //the collision normal will be the axis of least overlap
                float ov = (std::min(max_r1, max_r2) - std::max(min_r1, min_r2));
                if (ov < minOverlap)
                {
                    minOverlap = ov;
                    overlapNormal = axis;
                    object_ther_than_axis = o2;
                    object_that_is_axis = o1;
                    //std::cout << center1.X << "-" << center1.Y << std::endl;
                    if (Vec2D::DotProduct(overlapNormal, (center2-center1)) < 0)
                    {
                        overlapNormal = -overlapNormal;

                    }
                    if (o2->myColliderType == GColliderComp::ColliderType::Circle)
                    {
                        col.point = center2 - overlapNormal*o2->circleRadius;
                    }
                }
            }
        }
        else if (o1->myColliderType == GColliderComp::ColliderType::Circle)
        {
            if (o2->myColliderType == GColliderComp::ColliderType::Polygon)
            {
                Vec2D center1 = o1->gobject->transform.local_to_scene(o1->myCenter);
                Vec2D closest;
                float minDist = INFINITY;
                for (int j = 0; j < o2->myShape->numPoints; j++)
                {
                    Vec2D point = o2->gobject->transform.local_to_scene(o2->myShape->points[j]+o2->myCenter);
                    float dist = (point-center1).magnitude();
                    if (dist < minDist)
                    {
                        minDist = dist;
                        closest = point;
                    }
                }
                Vec2D axis = (closest - center1).getNormal();

                float centerproj = Vec2D::DotProduct(axis, center1);
                //float min_r1 = INFINITY; float max_r1 = -INFINITY;
                float min_r1 = centerproj - o1->circleRadius;
                float max_r1 = centerproj + o1->circleRadius;

                //2nd shape
                float min_r2 = INFINITY; float max_r2 = -INFINITY;
                for (int p = 0; p < o2->myShape->numPoints; p++)
                {
                    float proj = Vec2D::DotProduct(axis, o2->gobject->transform.local_to_scene(o2->myShape->points[p]+o2->myCenter));
                    min_r2 = std::min(min_r2, proj);
                    max_r2 = std::max(max_r2, proj);
                }

                if (!(max_r2 >= min_r1 && max_r1 >= min_r2)) {
                    col.depth = -1;
                    col.colliding = false;
                    return col;
                }

                float ov = (std::min(max_r1, max_r2) - std::max(min_r1, min_r2));
                if (ov < minOverlap && minDist < o1->circleRadius)
                {
                    // minOverlap = ov;
                    // overlapNormal = axis;
                    // object_ther_than_axis = o2;
                    // object_that_is_axis = o1;
                    // col.point = closest;
                    // if (Vec2D::DotProduct(overlapNormal, (closest-o1->gobject->transform.myScenePosition())) < 0)
                    // {
                    //     overlapNormal = -overlapNormal;
                    // }
                }
            }

            else if (o2->myColliderType == GColliderComp::ColliderType::Circle)
            {
                Vec2D center1 = o1->gobject->transform.local_to_scene(o1->myCenter);
                Vec2D center2 = o2->gobject->transform.local_to_scene(o2->myCenter);
                Vec2D dir = center2 - center1;

                float dist = dir.magnitude();

                if (dist < o2->circleRadius + o1->circleRadius)
                {
                    col.colliding = true;
                    col.normal = dir.getNormal();
                    col.col1 = o1;
                    col.col2 = o2;
                    col.depth = o2->circleRadius + o1->circleRadius - dist;
                    col.point = center1 + dir * (o1->circleRadius/dist);
                    return col;
                }
                else
                {
                    col.colliding = false;
                    return col;
                }
            }
        }
    }
    //std::cout << "yes" << std::endl;
    //to find the point of collision.
    //the point is considered the one that has gotten most inside the other shape
    //this is found by taking the projection of each point onto the normal of collision
    //the point with least value will be the one inside the other (least as in could be negative as well)

    //assume that origin is on the side thru which normal goes then points inside the shape will have negative projection
    col.colliding = true;
    col.depth = minOverlap;
    col.normal = overlapNormal;
    //std::cout << overlapNormal.X << "-" << overlapNormal.Y << std::endl;
    float proj = INFINITY;
    if (object_ther_than_axis->myColliderType == GColliderComp::ColliderType::Polygon && object_that_is_axis->myColliderType == GColliderComp::ColliderType::Polygon)
    {
        for (int i = 0; i < object_ther_than_axis->myShape->numPoints; i++)
        {
            Vec2D pt = object_ther_than_axis->gobject->transform.local_to_scene(object_ther_than_axis->myShape->points[i]+object_ther_than_axis->myCenter);
            float t = Vec2D::DotProduct(col.normal, pt);
            if (t<proj) {
                proj = t;
                col.point = pt;
            }
        }
    }
    // else if (object_ther_than_axis->myColliderType == GColliderComp::ColliderType::Circle)
    // {
    //     if (object_that_is_axis->myColliderType == GColliderComp::ColliderType::Polygon)
    //     {
    //         Vec2D normal = col.normal;
    //         float minDist = INFINITY;
    //         Vec2D point = object_ther_than_axis->gobject->transform.position + col.normal*object_ther_than_axis->circleRadius;
    //         Vec2D circleOrigin = object_ther_than_axis->gobject->transform.myScenePosition();
    //         for (int i = 0; i < object_that_is_axis->myShape->numPoints; i++)
    //         {
    //             Vec2D colpt = object_that_is_axis->gobject->transform.local_to_scene(object_that_is_axis->myShape->points[i]);
    //             float dist = Vec2D::Distance(circleOrigin, colpt);
    //             if (dist < object_ther_than_axis->circleRadius)
    //             {
    //                 minDist = dist;
    //                 col.normal = circleOrigin - colpt;
    //                 col.depth = object_ther_than_axis->circleRadius - dist;
    //             }
    //         }
    //         col.normal.normalize();
    //     }
    // }
    col.col1 = object_that_is_axis;
    col.col2 = object_ther_than_axis; //other_than_axis. i made a type now i dont want to change

    return col;
}

void GPhysics::HandleCollision(Collision col)
{
    if (!col.col1->gobject->getPhysicsEnabled())
    {
        if (!col.col2->gobject->getPhysicsEnabled())
        {
            return;
        }
        else
        {
            float multiplier = (col.col2->gobject->bounce + col.col1->gobject->bounce)/2 + 1;
            Vec2D vel_at_hit_point = col.col2->gobject->velocity_at_point(col.point);

            Vec2D r = col.point - col.col2->gobject->transform.myScenePosition();

            col.col2->gobject->transform.position = col.col2->gobject->transform.position + col.normal*col.depth;
            float vel_normal_scalar = Vec2D::DotProduct(col.normal, vel_at_hit_point);
            if (vel_normal_scalar > 0) return;
            Vec2D normalVel = col.normal*vel_normal_scalar;

            //applying coefficient of restitution formula
            float rxn = Vec2D::CrossProduct(r, col.normal);
            float j = multiplier * vel_normal_scalar;
            float invMass = (1.f*col.col2->gobject->invMass) + (rxn*rxn)*col.col2->gobject->invMomentOfInertia;
            j /= invMass;
            AddImpulseAtLocation(col.col2->gobject, col.point, -col.normal*j);
            vel_at_hit_point = col.col2->gobject->velocity_at_point(col.point);
            //friction
            float avg_friction = (col.col1->gobject->friction + col.col2->gobject->friction)/2;
            Vec2D tangent = Vec2D::GetPerpendicular(col.normal);

            //Vec2D tangent = (vel_at_hit_point - col.normal * Vec2D::DotProduct(vel_at_hit_point, col.normal));
            if (tangent.magnitude() < 1e-6f)
                return;
            tangent.normalize();

            //tangent = -tangent;
            float rxt = Vec2D::CrossProduct(r, tangent);
            float invMassTang = (1.f*col.col2->gobject->invMass) + (rxt*rxt)*col.col2->gobject->invMomentOfInertia;
            float jt = Vec2D::DotProduct(vel_at_hit_point, tangent)/invMassTang;
            float maxFric = avg_friction*std::abs(j);
            //if (maxFric < 0) maxFric = -maxFric;
            jt = std::clamp(jt, -maxFric, maxFric);
            // float vel_along_tangent = Vec2D::DotProduct(tangent, vel_at_hit_point);

            AddImpulseAtLocation(col.col2->gobject, col.point, tangent*(-jt));
            //std::cout << tangent.X << " - " << tangent.Y << ":  " << vel_at_hit_point.X<< " - " << vel_at_hit_point.Y << std::endl;

            //std::cout << col.col2->gobject->angularVelocity << std::endl;
        }
    }
    else
    {
        if (!col.col2->gobject->getPhysicsEnabled())
        {
            float multiplier = (col.col2->gobject->bounce + col.col1->gobject->bounce)/2 + 1;
            Vec2D vel_at_hit_point = col.col1->gobject->velocity_at_point(col.point);

            Vec2D r = col.point - col.col1->gobject->transform.myScenePosition();

            col.col1->gobject->transform.position = col.col1->gobject->transform.position - col.normal*col.depth;
            Vec2D normalVel = col.normal*Vec2D::DotProduct(col.normal, vel_at_hit_point);
            float vel_normal_scalar = Vec2D::DotProduct(col.normal, vel_at_hit_point);
            if (vel_normal_scalar < 0) return;
            //coeff of restitution formula j = -(1+e)*v
            float rxn = Vec2D::CrossProduct(r, col.normal);
            float j = multiplier * vel_normal_scalar;
            float invMass = (1.f*col.col1->gobject->invMass) + (rxn*rxn)*col.col1->gobject->invMomentOfInertia; //because impulse
            j /= invMass;
            AddImpulseAtLocation(col.col1->gobject, col.point, -col.normal*j);

            //implimenting friction
            float avg_friction = (col.col1->gobject->friction + col.col2->gobject->friction)/2;

            Vec2D tangent = Vec2D::GetPerpendicular(col.normal);
            float vel_along_tangent = Vec2D::DotProduct(tangent, vel_at_hit_point);

            float rxt = Vec2D::CrossProduct(r, tangent);
            float invMassTang = (1.f*col.col1->gobject->invMass) + (rxt*rxt)*col.col1->gobject->invMomentOfInertia;
            float jt = Vec2D::DotProduct(vel_at_hit_point, tangent)/invMassTang;
            float maxFric = avg_friction*std::abs(j);
            //if (maxFric < 0) maxFric = -maxFric;
            jt = -std::clamp(jt, -maxFric, maxFric);

            // if (vel_along_tangent > 0)
            // {
            //     tangent = -tangent;
            //     vel_along_tangent = - vel_along_tangent;
            // }
            AddImpulseAtLocation(col.col1->gobject, col.point, tangent*jt);//normalVel.magnitude()*avg_friction);

            // std::cout << jt << std::endl;
        }

        //both objects are rigid bodies
        else
        {
            //normal vector is from B to A - from 1 to 2
            //1 is the object whose side got intersected
            //a point on 2 got inside object 1
            //normal is pointing in the direction of 1 to 2 to push 2 out of 1

            float e = (col.col2->gobject->bounce + col.col1->gobject->bounce)/2;
            col.col1->gobject->transform.position = col.col1->gobject->transform.position - col.normal*col.depth/2;
            col.col2->gobject->transform.position = col.col2->gobject->transform.position + col.normal*col.depth/2;

            float j = -(1+e);

            Vec2D v1p = col.col1->gobject->velocity_at_point(col.point); //B
            Vec2D v2p = col.col2->gobject->velocity_at_point(col.point); //A

            Vec2D R2p = col.point - col.col2->gobject->transform.position; //Ra
            Vec2D R1p = col.point - col.col1->gobject->transform.position; //Rb
            Vec2D v21 = v2p-v1p; //from B to A

            float r2xn = Vec2D::CrossProduct(R2p, col.normal);
            float r1xn = Vec2D::CrossProduct(R1p, col.normal);
            j *= Vec2D::DotProduct(col.normal, v21);

            float invMass = (1.f*col.col1->gobject->invMass) + (1.f*col.col2->gobject->invMass);
            invMass += (r1xn*r1xn)*col.col1->gobject->invMomentOfInertia + (r2xn*r2xn)*col.col2->gobject->invMomentOfInertia;

            j /= invMass;

            AddImpulseAtLocation(col.col1->gobject, col.point, -col.normal*j);
            AddImpulseAtLocation(col.col2->gobject, col.point, col.normal*j);

            //friction
            float avg_friction = (col.col1->gobject->friction + col.col2->gobject->friction)/2;
            Vec2D tangent = Vec2D::GetPerpendicular(col.normal);

            float o1_tangent_vel = Vec2D::DotProduct(tangent, v1p);
            float o2_tangent_vel = Vec2D::DotProduct(tangent, v2p);

            float r2xt = Vec2D::CrossProduct(R2p, tangent);
            float r1xt = Vec2D::CrossProduct(R1p, tangent);

            float invMassTang = (1.f*col.col1->gobject->invMass) + (1.f/col.col2->gobject->invMass);
            invMassTang += (r1xt*r1xt)*col.col1->gobject->invMomentOfInertia + (r2xt*r2xt)*col.col2->gobject->invMomentOfInertia;

            float jt = Vec2D::DotProduct(v21, tangent)/invMassTang;


            float maxFric = avg_friction*std::abs(j);
            //if (maxFric < 0) maxFric = -maxFric;

            jt = std::clamp(jt, -maxFric, maxFric);

            float relative_tan_vel = o1_tangent_vel - o2_tangent_vel;
            // if (relative_tan_vel < 0)
            // {
            //     relative_tan_vel = -relative_tan_vel;
            //     tangent = -tangent;
            // }

            AddImpulseAtLocation(col.col1->gobject, col.point, tangent*jt);
            AddImpulseAtLocation(col.col2->gobject, col.point, -tangent*jt);


        }
    }



}

void GPhysics::AddImpulseAtLocation(GObject *const object, Vec2D location ,Vec2D force)
{
    Vec2D acc = force*object->invMass;
    Vec2D diff = location - object->transform.myScenePosition();
    float torque = Vec2D::CrossProduct(diff, force);
    object->angularVelocity += torque*object->invMomentOfInertia;
    //std::cout << acc.magnitude() << std::endl;
    object->velocity = object->velocity+acc;
}

void GPhysics::AddVelocityAtLocation(GObject *const object, Vec2D location ,Vec2D velocity)
{
    //Vec2D acc = force * (1/object->mass);
    Vec2D diff = location - object->transform.myScenePosition();
    float angular = Vec2D::CrossProduct(diff, velocity);
    object->angularVelocity += angular;
    //std::cout << torque << std::endl;
    object->velocity = object->velocity+velocity;
}

uint16_t GPhysics::registerCollider(GColliderComp *col)
{
    if (gColliderCount > MAX_COLLIDERS)
        return UINT16_MAX;

    gColliders[gColliderCount] = col;
    gColliderCount++;
    //std::cout << gColliderCount << std::endl;
    return 0;
}

void GPhysics::ApplyVelocities(float DeltaTime)
{
    for (auto& obj : myScene->sceneObjects)
    {
        if (!obj->getPhysicsEnabled()) continue;
        obj->transform.position = obj->transform.position + obj->velocity*DeltaTime;
        obj->transform.rotation = obj->transform.rotation + obj->angularVelocity*DeltaTime;
    }
}

void GPhysics::Damping(float DeltaTime)
{
    float linearDamp = 0.1f;
    float angularDamp = 0.05f;
    for (auto& obj : myScene->sceneObjects)
    {
        if (!obj->getPhysicsEnabled() || !obj->enabled) continue;
        obj->velocity = obj->velocity*(1-linearDamp*DeltaTime);
        obj->angularVelocity = obj->angularVelocity*(1-angularDamp*DeltaTime);
    }
}
