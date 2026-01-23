#pragma once
#include "Network.h"

static class SpaceShipMove_Calculator
{
public:

	static void Calcul_Forward(User* user, ServerNetwork* network);
	static void Calcul_Backward(User* user, ServerNetwork* network);
	static void Calcul_Left(User* user, ServerNetwork* network);
	static void Calcul_Right(User* user, ServerNetwork* network);
};

