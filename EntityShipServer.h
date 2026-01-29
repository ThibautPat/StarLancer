#pragma once
#include "pch.h"
#include "EntityServer.h"

class EntityShipServer : public EntityServer
{
public:
	EntityServer* LastKiller = nullptr;

	int life = 100;

	bool NeedToRespawn = false;
	float TimeToRespawn = 5;
	float TimeBeforeRespawn = 0;

	EntityShipServer();

	void OnCollide(EntityServer* entity) override;

	void Update(float dt) override;
};
