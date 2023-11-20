#pragma once
#include <windows.h>
#include "class.h"
#include "hackmath.h"
#include "main.h"


namespace hack
{
	void Aimbot(local local, Vec3 localpos3, Vec3 entitypos3);

	BOOL ESP();

	void SetAim(local local, float x, float y);
	
}