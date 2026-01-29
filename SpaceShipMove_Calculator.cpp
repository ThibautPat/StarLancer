#include "pch.h"
#include "SpaceShipMove_Calculator.h"

void SpaceShipMove_Calculator::Calcul_Forward(User* user, ServerNetwork* network)
{
    float speed = 0.3f;
    network->ListEntity[user->s_userID]->transform.pos.x += network->ListEntity[user->s_userID]->transform.dir.x * speed;
    network->ListEntity[user->s_userID]->transform.pos.y += network->ListEntity[user->s_userID]->transform.dir.y * speed;
    network->ListEntity[user->s_userID]->transform.pos.z += network->ListEntity[user->s_userID]->transform.dir.z * speed;
}

void SpaceShipMove_Calculator::Calcul_Backward(User* user, ServerNetwork* network)
{
    float speed = 0.3f;
    network->ListEntity[user->s_userID]->transform.pos.x -= network->ListEntity[user->s_userID]->transform.dir.x * speed;
    network->ListEntity[user->s_userID]->transform.pos.y -= network->ListEntity[user->s_userID]->transform.dir.y * speed;
    network->ListEntity[user->s_userID]->transform.pos.z -= network->ListEntity[user->s_userID]->transform.dir.z * speed;
}

void SpaceShipMove_Calculator::Calcul_Left(User* user, ServerNetwork* network)
{
    float speed = 0.3f;
    network->ListEntity[user->s_userID]->transform.pos.x -= network->ListEntity[user->s_userID]->transform.right.x * speed;
    network->ListEntity[user->s_userID]->transform.pos.y -= network->ListEntity[user->s_userID]->transform.right.y * speed;
    network->ListEntity[user->s_userID]->transform.pos.z -= network->ListEntity[user->s_userID]->transform.right.z * speed;
}

void SpaceShipMove_Calculator::Calcul_Right(User* user, ServerNetwork* network)
{
    float speed = 0.3f;
    network->ListEntity[user->s_userID]->transform.pos.x += network->ListEntity[user->s_userID]->transform.right.x * speed;
    network->ListEntity[user->s_userID]->transform.pos.y += network->ListEntity[user->s_userID]->transform.right.y * speed;
    network->ListEntity[user->s_userID]->transform.pos.z += network->ListEntity[user->s_userID]->transform.right.z * speed;
}
