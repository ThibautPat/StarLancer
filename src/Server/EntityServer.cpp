#include "pch.h"
#include "EntityServer.h"

void EntityServer::OnCollide(EntityServer* entity)
{
    if (!entity) return;

    // Direction de répulsion : de l'autre vers cette entité
    float dirX = transform.pos.x - entity->transform.pos.x;
    float dirY = transform.pos.y - entity->transform.pos.y;
    float dirZ = transform.pos.z - entity->transform.pos.z;

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
    float overlapX = minDistX - fabsf(transform.pos.x - entity->transform.pos.x);
    float overlapY = minDistY - fabsf(transform.pos.y - entity->transform.pos.y);
    float overlapZ = minDistZ - fabsf(transform.pos.z - entity->transform.pos.z);

    // Repousser sur l'axe avec le plus petit chevauchement
    if (overlapX > 0 && overlapX < overlapY && overlapX < overlapZ) {
        transform.pos.x += dirX * overlapX;
    }
    else if (overlapY > 0 && overlapY < overlapZ) {
        transform.pos.y += dirY * overlapY;
    }
    else if (overlapZ > 0) {
        transform.pos.z += dirZ * overlapZ;
    }
}

void EntityServer::Update(float dt)
{
    if (life <= 0)
    {
        IsDead = true;
        DeathCount++;
    }

    if (IsDead == false)
        return;

    TimeBeforeRespawn += dt;
    if (TimeBeforeRespawn >= TimeToRespawn)
    {
        NeedToRespawn = true;

        TimeBeforeRespawn = 0;

        transform.pos.x = 0;
        transform.pos.y = 0;
        transform.pos.z = 0;

        IsDead = false;
    }
}