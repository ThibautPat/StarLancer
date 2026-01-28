#pragma once
#include "EntityClient.h"
class EntityBulletClient : public EntityClient
{
public:
	uint32_t OwnerID;
	XMFLOAT3 ownerBULLET_FORWARD = { 0.5f, 0.0f, 0.0f };
};

