#pragma once
#include "pch.h"
#include "EntityServer.h"

class EntityBulletServer : public EntityServer
{
	int damage = 10;
public:

	EntityServer* Owner = nullptr;
	XMFLOAT3 ownerBULLET_FORWARD = { 0.5f, 0.0f, 0.0f };

	float LifeTime = 5;
	const float BULLET_SPEED = 30.f;

	EntityBulletServer();

	void OnCollide(EntityServer* entity) override;

	void Update(float dt) override;
};
