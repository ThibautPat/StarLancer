#pragma once
#include "pch.h"
#include "DataProtocol.h"

class EntityServer
{
public:
	uint32_t entityID;
	EntityType entityType;
	//POS

	cpu_transform transform;

	float currentYaw = 0.0f;
	float currentPitch = 0.0f;
	float currentRoll = 0.0f;

	//SIZE COLLIDER AABB

	XMFLOAT3 minAABB;
	XMFLOAT3 maxAABB;

	//DATA
	int life = 50;
	bool IsDead = false;
	float TimeToRespawn = 5;
	float TimeBeforeRespawn = 0;
	
	int DeathCount = 0;
	int KillCount = 0;
	int Score = 0;

	virtual void OnCollide(EntityServer* entity);
};

