#pragma once
#include "pch.h"
#include "DataProtocol.h"

class EntityServer
{
public:
	virtual ~EntityServer() = default;

	uint32_t entityID;
	EntityType entityType;

	cpu_transform transform;
	float currentYaw = 0.0f;
	float currentPitch = 0.0f;
	float currentRoll = 0.0f;

	XMFLOAT3 minAABB;
	XMFLOAT3 maxAABB;

	bool IsDead = false;
	
	virtual void Update(float dt) = 0;
	virtual void OnCollide(EntityServer* entity) = 0;
};

