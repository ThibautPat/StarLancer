#pragma once
#include "pch.h"
#include "EntityServer.h"


class EntityBulletServer : public EntityServer
{
	int damage = 10;
	public:
	XMFLOAT3 ownerBULLET_FORWARD = { 0.5f, 0.0f, 0.0f };
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

