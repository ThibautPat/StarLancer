#pragma once
#include "Network.h"

static class SpaceShipMove_Calculator
{
public:

	static void Calcul_Forward(User* user);
	static void Calcul_Backward(User* user);
	static void Calcul_Left(User* user);
	static void Calcul_Right(User* user);
};

