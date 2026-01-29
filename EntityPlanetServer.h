#pragma once
#include "pch.h"
#include "EntityServer.h"

class EntityPlanetServer : public EntityServer
{
public:

	EntityServer* Target = nullptr;
	float radius;
	float angle;

	void OnCollide(EntityServer* entity) override;

	void Update(float dt) override;
};
