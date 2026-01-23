#include "pch.h"
#include "SpaceShipMove_Calculator.h"

void SpaceShipMove_Calculator::Calcul_Forward(User* user,ServerNetwork* network)
{
    float speed = 0.1f;
    network->ListEntity[user->s_userID]->PosZ -= speed;

}

void SpaceShipMove_Calculator::Calcul_Backward(User* user, ServerNetwork* network)
{
    float speed = 0.1f;
    network->ListEntity[user->s_userID]->PosZ += speed;
    
}

void SpaceShipMove_Calculator::Calcul_Left(User* user, ServerNetwork* network)
{
    float speed = 0.1f;
    network->ListEntity[user->s_userID]->PosX += speed;

    
}

void SpaceShipMove_Calculator::Calcul_Right(User* user, ServerNetwork* network)
{
    float speed = 0.1f;
    network->ListEntity[user->s_userID]->PosX -= speed;
    
}

