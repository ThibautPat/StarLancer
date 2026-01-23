#include "pch.h"
#include "SpaceShipMove_Calculator.h"

void SpaceShipMove_Calculator::Calcul_Forward(User* user)
{
    float speed = 0.1f;
    user->s_EntityData->PosZ -= speed;
    user->s_EntityData->minAABB.z -= speed;
	user->s_EntityData->maxAABB.z -= speed;
}

void SpaceShipMove_Calculator::Calcul_Backward(User* user)
{
    float speed = 0.1f;
    user->s_EntityData->PosZ += speed;
    user->s_EntityData->minAABB.z += speed;
    user->s_EntityData->maxAABB.z += speed;
}

void SpaceShipMove_Calculator::Calcul_Left(User* user)
{
    float speed = 0.1f;
    user->s_EntityData->PosX += speed;
    user->s_EntityData->minAABB.x += speed;
    user->s_EntityData->maxAABB.x += speed;
}

void SpaceShipMove_Calculator::Calcul_Right(User* user)
{
    float speed = 0.1f;
    user->s_EntityData->PosX -= speed;
    user->s_EntityData->minAABB.x -= speed;
    user->s_EntityData->maxAABB.x -= speed;
}

