#include "pch.h"
#include "EntityBulletServer.h"

EntityBulletServer::EntityBulletServer()
{
	entityType = EntityType::BULLET;

	minAABB = { -0.1f, -0.1f, -0.1f };
	maxAABB = { 0.1f, 0.1f, 0.1f };
}

void EntityBulletServer::OnCollide(EntityServer* entity)
{
	IsDead = true;
	entity->life -= damage;
	if (entity->life < 0)
	{
		entity->life = 0;
		entity->LastKiller = Owner;
	}
}

void EntityBulletServer::Update(float dt) 
{
	transform.pos.x += ownerBULLET_FORWARD.x * BULLET_SPEED * dt;
	transform.pos.y += ownerBULLET_FORWARD.y * BULLET_SPEED * dt;
	transform.pos.z += ownerBULLET_FORWARD.z * BULLET_SPEED * dt;

	LifeTime -= dt;
	if (LifeTime <= 0)
		IsDead = true;
}