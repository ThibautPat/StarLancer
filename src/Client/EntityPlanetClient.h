#pragma once
#include "EntityClient.h"
#include "pch.h"
class EntityPlanetClient : public EntityClient
{
public:
	uint32_t IDEntityTarget;
	float radius;
	float angle;
	PlanetType type;
	void Update_EntityClient(float dt) override
	{
		pEntity->transform.OrbitAroundAxis(App::GetInstance().GetEntitie(IDEntityTarget)->pEntity->transform.pos, CPU_UP, 0.01, angle);
	}
};

