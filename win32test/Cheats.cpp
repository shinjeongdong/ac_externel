#include "Cheats.h"
#include <stdio.h>


void Cheats::Run()
{
	local localentity;
	ReadProcessMemory(hProc, (LPCVOID)(0x400000 + 0x18AC00), (LPVOID)&localentity.player_base, sizeof(int), 0);
	entity entity;

	float min_dis = 1000000;
	float dis = 0;
	Vec3 x = {0,0,0};
	Vec3 y = {0,0,0};
	for (int i = 0; i < 30; i++)
	{
		
		
		ReadProcessMemory(hProc, (LPCVOID)(0x400000 + 0x191fcc), (LPVOID)&entity.entity_base, sizeof(int), 0);
		ReadProcessMemory(hProc, (LPCVOID)(entity.entity_base + (i * 4)), (LPVOID)&entity.entity_base, sizeof(int), 0);
		if (entity.entity_base == 0)
		{
			continue;
		}
		
		entity.GetHealth();
		localentity.GetViewMartix();
		if (entity.health <= 0 || entity.health > 100)
		{
			continue;
		}

		localentity.GetPos();
		entity.GetPos();

		hack::ESP(entity,localentity);



		
		localentity.GetViewangle();

		//Aimbot Data
		dis = math::GetDistance(localentity.pos3, entity.pos3); // 거리 구하는법

		
		if (min_dis > dis)
		{
			min_dis = dis;
			x = entity.pos3;
			y = localentity.pos3;
		}
	
	}


	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		if ((entity.pos3.x != 0))
		{
			hack::Aimbot(localentity,y, x);
		}
	}

}
