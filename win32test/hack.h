#pragma once
#include <windows.h>
#include "class.h"
#include "hackmath.h"
#include "main.h"
#include "ESP1.h"

namespace hack
{
	void Aimbot(local local, Vec3 localpos3, Vec3 entitypos3);

	BOOL ESP(entity entity, local local);

	void SetAim(local local, float x, float y);
	
}