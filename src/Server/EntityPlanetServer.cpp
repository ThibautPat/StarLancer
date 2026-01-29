#include "pch.h"
#include "EntityPlanetServer.h"
#include "EntityShipServer.h"

void EntityPlanetServer::OnCollide(EntityServer* entity)
{

	if (entity->entityType != EntityType::SPACESHIP)
		return;

	EntityShipServer* ship = dynamic_cast<EntityShipServer*>(entity);

	
	ship->life = 0;
	
}

void EntityPlanetServer::Update(float dt)
{
	transform.OrbitAroundAxis(Target->transform.pos, CPU_UP, 0, angle);

}