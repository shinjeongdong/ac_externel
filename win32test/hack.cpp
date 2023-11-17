#include "hack.h"
#include <stdio.h>

void hack::Aimbot(local local, entity entity)
{
	float x_angle = 0;
	float y_angle = 0;
	float min_err = 1000000;
	float err = 0;
	float min_x_angle = 0;
	float min_y_angle = 0;

    x_angle = math::Get2Ddegree(local.pos3, entity.pos3);
    y_angle = math::GetYawdegree(local.pos3, entity.pos3);

	err = math::GetDistance(local.pos3, entity.pos3); // 거리 구하는법

	if (min_err > err)
	{
		min_err = err;
		min_x_angle = x_angle;
		min_y_angle = y_angle;
	}
    
	SetAim(local, min_x_angle, min_y_angle);
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


