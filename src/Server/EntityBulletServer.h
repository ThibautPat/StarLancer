#pragma once
#include "pch.h"
#include "EntityServer.h"


class EntityBulletServer : public EntityServer
{
	public:
	EntityBulletServer()
	{
		entityType = EntityType::BULLET;

		minAABB = { -0.1f, -0.1f, -0.1f };
		maxAABB = { 0.1f, 0.1f, 0.1f };
	}

	void OnCollide(EntityServer* entity) override
	{
		// Implémentation spécifique pour le bullet
		// Par exemple, détruire le bullet ou infliger des dégâts à l'entité touchée
	}
};

