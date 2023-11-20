#include "hack.h"
#include <stdio.h>

void hack::Aimbot(local local,Vec3 localpos3, Vec3 entitypos3)
{
	float x_angle = 0;
	float y_angle = 0;

    x_angle = math::Get2Ddegree(localpos3, entitypos3);
    y_angle = math::GetYawdegree(localpos3, entitypos3);

    
	SetAim(local, x_angle, y_angle);
}

void hack::SetAim(local local,float x, float y)
{
	WriteProcessMemory(hProc, LPVOID(local.player_base + 0x34), &x, sizeof(x), 0);
	WriteProcessMemory(hProc, LPVOID(local.player_base + 0x38), &y, sizeof(y), 0);
}


BOOL hack::ESP()
{
    return 0;
}


