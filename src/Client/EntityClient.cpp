#include "pch.h"
#include "EntityClient.h"

void EntityClient::Update_EntityClient(float dt)
{
	if (life <= 0)
	{
		IsDead = true;
	}
	float ratio = (life - 0) / (50 - 0);
	pEntity->pMaterial->color = { ratio, 0, 0 };
}

void EntityClient::Respawn()
{
	if (IsDead == true)
	{
		IsDead = false;
		life = 50;
		pEntity->pMaterial->color = { 1, 1, 1 };
	}
}