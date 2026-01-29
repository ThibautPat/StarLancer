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

void EntityClient::Respawn(int _life)
{
	if (IsDead == true)
	{
		IsDead = false;
		life = _life;
	}
}

void EntityClient::clearEntity()
{
	cpuEngine.Release(pEntity);
}
