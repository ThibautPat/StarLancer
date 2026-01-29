#pragma once
#include "pch.h"
#include "EntityServer.h"

class EntityBulletServer : public EntityServer
{
	int damage = 10;
public:

	EntityServer* Owner;
	XMFLOAT3 ownerBULLET_FORWARD = { 0.5f, 0.0f, 0.0f };
	float LifeTime = 5;

	EntityBulletServer()
	{
		entityType = EntityType::BULLET;

		minAABB = { -0.1f, -0.1f, -0.1f };
		maxAABB = { 0.1f, 0.1f, 0.1f };
	}

	void OnCollide(EntityServer* entity) override
	{
		entity->life -= damage;
		if (entity->life < 0)
			entity->life = 0;
	}

	void Update(float dt) override
	{
		const float BULLET_SPEED = 20.f;

		transform.pos.x += ownerBULLET_FORWARD.x * BULLET_SPEED * dt;
		transform.pos.y += ownerBULLET_FORWARD.y * BULLET_SPEED * dt;
		transform.pos.z += ownerBULLET_FORWARD.z * BULLET_SPEED * dt;
		LifeTime -= dt;
		if (LifeTime <= 0)
		{
			IsDead = true;
		}
	} 
};

