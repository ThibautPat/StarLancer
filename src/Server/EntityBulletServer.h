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
};

