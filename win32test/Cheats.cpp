#include "Cheats.h"
#include "main.h"
#include "hack.h"
#include <stdio.h>


void Cheats::Run()
{
	local localentity;
	ReadProcessMemory(hProc, (LPCVOID)(0x400000 + 0x18AC00), (LPVOID)&localentity.player_base, sizeof(int), 0);

	for (int i = 0; i < 30; i++)
	{
		entity entity;
		localentity.GetPos();
		ReadProcessMemory(hProc, (LPCVOID)(0x400000 + 0x191fcc), (LPVOID)&entity.entity_base, sizeof(int), 0);
		ReadProcessMemory(hProc, (LPCVOID)(entity.entity_base + (i * 4)), (LPVOID)&entity.entity_base, sizeof(int), 0);
		if (entity.entity_base == 0)
		{
			continue;
		}
		entity.GetPos();
		entity.GetHealth();
		localentity.GetViewangle();


		if (entity.health <= 0 || entity.health > 100)
		{
			continue;
		}



		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			hack::Aimbot(localentity, entity);
		}
	}

	return;
}
