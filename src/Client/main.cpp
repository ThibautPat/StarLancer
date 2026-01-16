#include "pch.h"
#include "main.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	CPU_RUN(1024, 576);
	return 0;
}