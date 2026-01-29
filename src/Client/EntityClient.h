#pragma once

class EntityClient
{
public:
	bool bullet = false;
	uint32_t entityID;
	cpu_entity* pEntity;

	int life = 50;
	bool IsDead = false;

	virtual void Update_EntityClient(float dt);
	void clearEntity();
	void Respawn(int _life);
};

