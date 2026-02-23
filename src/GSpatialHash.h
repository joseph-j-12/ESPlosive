#ifndef HEADER_SPATIALHASH
#define HEADER_SPATIALHASH
#include <stdint.h>
//#include "GScene.h"
//#include "GPhysics.h"

constexpr int MAX_COLLIDERS = 256;
constexpr int MAX_CELLS_PER_COLLIDER = 4;
constexpr int MAX_BUCKETS = 512;
constexpr int MAX_ENTRIES = MAX_COLLIDERS * MAX_CELLS_PER_COLLIDER;
constexpr float CELL_SIZE = 100.0f; 
class GScene;
class GPhysics;
class GColliderComp;
/*
Spatial hashing for faster collision detection
Uses fixed arrays instead of any other data type to avoid memory fragmentation
Uses a linked list like approach for checking collision for only nearby objects


*/
class GSpatialHash
{

    public:
    //
    struct HashEntry
    {
        uint16_t colliderIndex;
        int16_t next;
    };

    int16_t bucketHead[MAX_BUCKETS];
    HashEntry entries[MAX_ENTRIES];
    int16_t entryCount;


    inline int HashCell(int x, int y)
    {
        uint32_t h = (uint32_t)(x * 73856093u) ^ (uint32_t)(y * 19349663u);
        return h & (MAX_BUCKETS - 1);
    }

    void InsertCollider(GColliderComp* col, uint16_t colliderIndex);

    inline int WorldToCell(float v)
    {
        int i = (int)v;
        return (v < 0 && v != i) ? (i - 1) : i;
    }

    void Clear()
    {
        for (int i = 0; i < MAX_BUCKETS; i++)
            bucketHead[i] = -1;

        entryCount = 0;
    }
};



#endif