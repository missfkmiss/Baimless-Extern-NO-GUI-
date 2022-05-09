#pragma once
#include <Windows.h>

class Globals
{
public:

	bool active = true;
	bool trigger = false;
	bool bhop = false;
	bool glow = false;
	bool activeAimbot = false;
	HANDLE hProcess;
	uintptr_t modulebase = 0;
	uintptr_t engineAddress = 0;
	float smooth = 20.f;
	HDC hdc;
	int screenX;
	int screenY;

}; Globals pGlobals;