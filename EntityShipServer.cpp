#include "pch.h"
#include "EntityShipServer.h"
#include <cmath>

EntityShipServer::EntityShipServer()
{
}

void EntityShipServer::OnCollide(EntityServer* entity)
{
    if (!entity)
        return;

    float dirX = transform.pos.x - entity->transform.pos.x;
    float dirY = transform.pos.y - entity->transform.pos.y;
    float dirZ = transform.pos.z - entity->transform.pos.z;

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

    float thisHalfX = (maxAABB.x - minAABB.x) * 0.5f;
    float thisHalfY = (maxAABB.y - minAABB.y) * 0.5f;
    float thisHalfZ = (maxAABB.z - minAABB.z) * 0.5f;

    float otherHalfX = (entity->maxAABB.x - entity->minAABB.x) * 0.5f;
    float otherHalfY = (entity->maxAABB.y - entity->minAABB.y) * 0.5f;
    float otherHalfZ = (entity->maxAABB.z - entity->minAABB.z) * 0.5f;

    float minDistX = thisHalfX + otherHalfX;
    float minDistY = thisHalfY + otherHalfY;
    float minDistZ = thisHalfZ + otherHalfZ;

    float overlapX = minDistX - fabsf(transform.pos.x - entity->transform.pos.x);
    float overlapY = minDistY - fabsf(transform.pos.y - entity->transform.pos.y);
    float overlapZ = minDistZ - fabsf(transform.pos.z - entity->transform.pos.z);

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

void EntityShipServer::Update(float dt)
{
    if (life <= 0)
    {
        IsDead = true;
    }

    if (IsDead == false)
        return;

    TimeBeforeRespawn += dt;
    if (TimeBeforeRespawn >= TimeToRespawn)
    {
        NeedToRespawn = true;
        TimeBeforeRespawn = 0;
    }
}
