#include "pch.h"
#include "SpaceShipMove_Calculator.h"

void SpaceShipMove_Calculator::Calcul_Forward(User* user)
{
    float speed = 0.1f;
    user->s_EntityData->PosZ -= speed;
}

void SpaceShipMove_Calculator::Calcul_Backward(User* user)
{
    float speed = 0.1f;
    user->s_EntityData->PosZ += speed;
}

