#include "GSpatialHash.h"
#include "GScene.h"
#include "GPhysics.h"
#include "GColliderComp.h"
#include <iostream>
void GSpatialHash::InsertCollider(GColliderComp* col, uint16_t colliderIndex)
{
    if(!col->CalculateBoundingBox()) {return;}
    int minX = WorldToCell(col->boundingBoxMin.X / CELL_SIZE);
    int minY = WorldToCell(col->boundingBoxMin.Y / CELL_SIZE);
    int maxX = WorldToCell(col->boundingBoxMax.X / CELL_SIZE);
    int maxY = WorldToCell(col->boundingBoxMax.Y / CELL_SIZE);
    for (int i = minX; i <= maxX; i++)
    {
        for (int j = minY; j<= maxY; j++)
        {
            if (entryCount >= MAX_ENTRIES)
            {
                return;
            }
            int bucket = HashCell(i,j);

            int16_t e = entryCount++;

            entries[e].colliderIndex = colliderIndex;
            entries[e].next = bucketHead[bucket];
            bucketHead[bucket] = e;
        }
    }
}
