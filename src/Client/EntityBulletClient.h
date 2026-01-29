#pragma once
#include "EntityClient.h"
#include "pch.h"
class EntityBulletClient : public EntityClient
{
public:
	uint32_t OwnerID;
	XMFLOAT3 ownerBULLET_FORWARD = { 0.5f, 0.0f, 0.0f };
	float LifeTime = 5;

	void Update_EntityClient(float dt) override
	{
		const float BULLET_SPEED = 20.f;

		pEntity->transform.pos.x += ownerBULLET_FORWARD.x * BULLET_SPEED * dt;
		pEntity->transform.pos.y += ownerBULLET_FORWARD.y * BULLET_SPEED * dt;
		pEntity->transform.pos.z += ownerBULLET_FORWARD.z * BULLET_SPEED * dt;

		LifeTime -= dt;
		if (LifeTime <= 0)
			IsDead = true;
	}
};

