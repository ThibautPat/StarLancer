#pragma once
#include "pch.h"

class EntityServer
{
public:
	uint32_t entityID;
	
	//POS
	float PosX = 0;
	float PosY = 0;
	float PosZ = 0;

	//SIZE COLLIDER AABB

	XMFLOAT3 minAABB;
	XMFLOAT3 maxAABB;

	virtual void OnCollide(EntityServer* entity);

};

