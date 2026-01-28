#pragma once
#include "EntityClient.h"
class EntityBulletClient : public EntityClient
{
public:
	uint32_t OwnerID;

	void Update_EntityClient(float dt) override
	{
		pEntity->transform.pos.z -= 0.5f * dt;
	}
};

