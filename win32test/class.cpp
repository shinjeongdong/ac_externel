#include <windows.h>
#include "class.h"
#include "main.h"

BOOL local::GetHealth()
{

	return ReadProcessMemory(hProc, (LPCVOID)(player_base + 0xEC), (LPVOID) & (this->health), sizeof(int), 0);
}

BOOL local::GetPos()
{


	return ReadProcessMemory(hProc, (LPCVOID)(player_base + 0x28), (LPVOID) & (this->pos3), sizeof(Vec3), 0);
}

BOOL local::GetViewangle()
{
	return ReadProcessMemory(hProc, (LPCVOID)(player_base + 0x34), (LPVOID) & (this->viewangle), sizeof(Vec2), 0);
}

BOOL entity::GetHealth()
{
	return ReadProcessMemory(hProc, (LPCVOID)(entity_base + 0xEC), (LPVOID) & (this->health), sizeof(int), 0);
}

BOOL entity::GetPos()
{
	return ReadProcessMemory(hProc, (LPCVOID)(entity_base + 0x28), (LPVOID) & (this->pos3), sizeof(Vec3), 0);
}



