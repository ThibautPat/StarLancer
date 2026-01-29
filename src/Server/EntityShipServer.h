#pragma once
#include "pch.h"
#include "EntityServer.h"

class EntityShipServer : public EntityServer
{
public:
	EntityServer* LastKiller;

	int life = 50;

	bool NeedToRespawn = false;
	float TimeToRespawn = 5;
	float TimeBeforeRespawn = 0;

	EntityShipServer();

	void OnCollide(EntityServer* entity) override;

	void Update(float dt) override;
};