#pragma once
#include "pch.h"
#include "EntityServer.h"

class EntityBulletServer : public EntityServer
{
	int damage = 10;

public:

	EntityBulletServer()
	{
		entityType = EntityType::BULLET;

		minAABB = { -0.1f, -0.1f, -0.1f };
		maxAABB = { 0.1f, 0.1f, 0.1f };
	}

	void OnCollide(EntityServer* entity) override
	{
		entity->life -= damage;
		if (entity->life <= 0)
		{
			entity->IsDead = true;
			entity->DeathCount++;
		}
	}

	void Update(float dt) override
	{
		const float BULLET_SPEED = 0.5f;
		PosZ -= BULLET_SPEED * dt;
	} 
};

