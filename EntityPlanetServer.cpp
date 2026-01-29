#include "pch.h"
#include "EntityPlanetServer.h"
#include "EntityShipServer.h"

void EntityPlanetServer::OnCollide(EntityServer* entity)
{
	// NO ENOUGH TIME
	//if (entity->entityType != EntityType::SPACESHIP)
	//	return;

	//EntityShipServer* ship = dynamic_cast<EntityShipServer*>(entity);

	//ship->life = 0;
}

void EntityPlanetServer::Update(float dt)
{
	if (Target)
		transform.OrbitAroundAxis(Target->transform.pos, { 0.0f, 1.0f, 0.0f }, 0, angle);
}
