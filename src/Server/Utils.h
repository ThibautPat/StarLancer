#include "pch.h"
#include <random>

static int randomBetweenMinus25And25() 
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(-25, 25);
    return dist(gen);
}