#include "pch.h"
#include "EntityServer.h"

void EntityServer::OnCollide(EntityServer* entity)
{
    if (!entity) return;

    XMFLOAT3 thisCenter = {
        (minAABB.x + maxAABB.x) * 0.5f,
        (minAABB.y + maxAABB.y) * 0.5f,
        (minAABB.z + maxAABB.z) * 0.5f
    };

    XMFLOAT3 otherCenter = {
        (entity->minAABB.x + entity->maxAABB.x) * 0.5f,
        (entity->minAABB.y + entity->maxAABB.y) * 0.5f,
        (entity->minAABB.z + entity->maxAABB.z) * 0.5f
    };

    XMFLOAT3 direction = {
        thisCenter.x - otherCenter.x,
        thisCenter.y - otherCenter.y,
        thisCenter.z - otherCenter.z
    };

    float magnitude = sqrtf(
        direction.x * direction.x +
        direction.y * direction.y +
        direction.z * direction.z
    );

    if (magnitude < 0.0001f) {
        direction = { 1.0f, 0.0f, 0.0f };
        magnitude = 1.0f;
    }

    direction.x /= magnitude;
    direction.y /= magnitude;
    direction.z /= magnitude;

    float overlapX = ((maxAABB.x - minAABB.x) + (entity->maxAABB.x - entity->minAABB.x)) * 0.5f
        - fabsf(thisCenter.x - otherCenter.x);
    float overlapY = ((maxAABB.y - minAABB.y) + (entity->maxAABB.y - entity->minAABB.y)) * 0.5f
        - fabsf(thisCenter.y - otherCenter.y);
    float overlapZ = ((maxAABB.z - minAABB.z) + (entity->maxAABB.z - entity->minAABB.z)) * 0.5f
        - fabsf(thisCenter.z - otherCenter.z);

    float minOverlap = overlapX;
    if (overlapY < minOverlap) minOverlap = overlapY;
    if (overlapZ < minOverlap) minOverlap = overlapZ;

    float repulsionForce = 0.5f;

    if (minOverlap == overlapX) {
        PosX += (direction.x > 0 ? 1.0f : -1.0f) * overlapX * repulsionForce;
    }
    else if (minOverlap == overlapY) {
        PosY += (direction.y > 0 ? 1.0f : -1.0f) * overlapY * repulsionForce;
    }
    else {
        PosZ += (direction.z > 0 ? 1.0f : -1.0f) * overlapZ * repulsionForce;
    }

    XMFLOAT3 aabbSize = {
        maxAABB.x - minAABB.x,
        maxAABB.y - minAABB.y,
        maxAABB.z - minAABB.z
    };

    minAABB = { PosX, PosY, PosZ };
    maxAABB = {
        PosX + aabbSize.x,
        PosY + aabbSize.y,
        PosZ + aabbSize.z
    };
}