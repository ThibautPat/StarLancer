#pragma once
#include "pch.h"
#include "EntityServer.h"


class EntityBulletServer : public EntityServer
{

	void OnCollide(EntityServer* entity) override
	{
		// Implémentation spécifique pour le bullet
		// Par exemple, détruire le bullet ou infliger des dégâts à l'entité touchée
	}
};

