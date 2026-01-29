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
		// if (pEntity->pMaterial == nullptr)
		// {
		// 	pEntity->pMaterial = new cpu_material();
		// }
		// pEntity->pMaterial->color = { 1, 1, 1 };

		IsDead = false;
		life = _life;
	}
}

void EntityClient::clearEntity()
{
	if (!IsDead)
	{
		return;
	}

	App::GetInstance().GetEntitiesList().erase(entityID);
	
	delete pEntity;

}
