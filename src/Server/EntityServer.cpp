#include "pch.h"
#include "EntityServer.h"


void EntityServer::OnCollide(EntityServer* entity)
{
    if (!entity) return;

    // Direction de répulsion : de l'autre vers cette entité
    float dirX = PosX - entity->PosX;
    float dirY = PosY - entity->PosY;
    float dirZ = PosZ - entity->PosZ;

    // Normaliser
    float length = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);

    if (length < 0.001f) {
        dirX = 1.0f;
        dirY = 0.0f;
        dirZ = 0.0f;
    }
    else {
        dirX /= length;
        dirY /= length;
        dirZ /= length;
    }

    // Calculer les tailles
    float thisHalfX = (maxAABB.x - minAABB.x) * 0.5f;
    float thisHalfY = (maxAABB.y - minAABB.y) * 0.5f;
    float thisHalfZ = (maxAABB.z - minAABB.z) * 0.5f;

    float otherHalfX = (entity->maxAABB.x - entity->minAABB.x) * 0.5f;
    float otherHalfY = (entity->maxAABB.y - entity->minAABB.y) * 0.5f;
    float otherHalfZ = (entity->maxAABB.z - entity->minAABB.z) * 0.5f;

    // Distance minimale nécessaire
    float minDistX = thisHalfX + otherHalfX;
    float minDistY = thisHalfY + otherHalfY;
    float minDistZ = thisHalfZ + otherHalfZ;

    // Chevauchement actuel
    float overlapX = minDistX - fabsf(PosX - entity->PosX);
    float overlapY = minDistY - fabsf(PosY - entity->PosY);
    float overlapZ = minDistZ - fabsf(PosZ - entity->PosZ);

    // Repousser sur l'axe avec le plus petit chevauchement
    if (overlapX > 0 && overlapX < overlapY && overlapX < overlapZ) {
        PosX += dirX * overlapX;
    }
    else if (overlapY > 0 && overlapY < overlapZ) {
        PosY += dirY * overlapY;
    }
    else if (overlapZ > 0) {
        PosZ += dirZ * overlapZ;
    }
}