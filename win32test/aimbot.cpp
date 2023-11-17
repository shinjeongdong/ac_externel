#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <vector>


#define x_offset 0x28
#define z_offset 0x2c
#define y_offset 0x30
#define pit_offset 0x34
#define yaw_offset 0x38

HANDLE hTargetProc;

uintptr_t player_base;

uintptr_t entity_base;

typedef struct Vec3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
} Vec3;

typedef struct Matrix {
	float matrix[16];
};

class Player {

public:

	
	int base;
	int health;
	int e_base;
	Vec3 vec;
	float pitch;
	float yaw;
	
	Player() : base(0), health(0), e_base(0), vec{ 0.0f, 0.0f, 0.0f }, pitch(0.0f), yaw(0.0f) {}

	Player(HANDLE phandle)
	{
		ReadProcessMemory(phandle, (LPCVOID)player_base, &base, sizeof(base), 0);
		ReadProcessMemory(phandle, (LPCVOID)(base + x_offset), &vec.x, sizeof(vec.x), 0);
		ReadProcessMemory(phandle, (LPCVOID)(base + z_offset), &vec.z, sizeof(vec.y), 0);
		ReadProcessMemory(phandle, (LPCVOID)(base + y_offset), &vec.y, sizeof(vec.z), 0);
		ReadProcessMemory(phandle, (LPCVOID)(base + pit_offset), &pitch, sizeof(pitch), 0);
		ReadProcessMemory(phandle, (LPCVOID)(base + yaw_offset), &yaw, sizeof(yaw), 0);
		
	}

	Player(HANDLE phandle,int i)
	{
		
		
		ReadProcessMemory(phandle, (LPCVOID)entity_base, &e_base, sizeof(e_base), 0);
		ReadProcessMemory(phandle, (LPCVOID)(e_base + (i * 4)), &e_base, sizeof(e_base), 0);
		ReadProcessMemory(phandle, (LPCVOID)(e_base + 0xEC), &health, sizeof(health), 0);
		ReadProcessMemory(phandle, (LPCVOID)(e_base + x_offset), &vec.x, sizeof(vec.x), 0);
		ReadProcessMemory(phandle, (LPCVOID)(e_base + z_offset), &vec.z, sizeof(vec.y), 0);
		ReadProcessMemory(phandle, (LPCVOID)(e_base + y_offset), &vec.y, sizeof(vec.z), 0);
		
		
	}

};



DWORD GetProcId(const char* procName)
{
	DWORD pid = 0;
	HANDLE hProcSnap;
	PROCESSENTRY32 procEntry;

	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hProcSnap == INVALID_HANDLE_VALUE)
	{
		printf("hProc ��� ��� ����");
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hProcSnap, &procEntry);
	do
	{
		if (!_stricmp(procEntry.szExeFile, procName))
		{
			pid = procEntry.th32ProcessID;
			break;
		}
	} while (Process32Next(hProcSnap, &procEntry));
	CloseHandle(hProcSnap);

	return pid;
}

uintptr_t GetMoudleBaseAddress(const char* modName, DWORD pid)
{
	uintptr_t modBaseAddr = NULL;
	HANDLE hModSnap;
	MODULEENTRY32 modEntry;

	hModSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);

	if (hModSnap == INVALID_HANDLE_VALUE)
	{
		printf("hModSnap ��� ��� ����");
	}

	modEntry.dwSize = sizeof(LPMODULEENTRY32);
	Module32First(hModSnap, &modEntry);
	do
	{
		if (!_stricmp(modEntry.szModule, modName))
		{
			modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
			break;
		}
	} while (Module32Next(hModSnap, &modEntry));

	CloseHandle(hModSnap);

	return modBaseAddr;
}



float Get2Ddegree(Player player, Player entityplayer) //������ ������(2d)������ ���� ���ϱ�
{
	float x = player.vec.x - entityplayer.vec.x;
	float z = player.vec.z - entityplayer.vec.z;
	float c = 270;

	if (x < 0)
	{
		c = 90;
	}

	return c + atan(z / x) * 180 / 3.141592;
}

float GetYawdegree(Player player, Player entityplayer) // ������ ������(������)������ ���� ���ϱ�
{
	float xz = sqrt(pow(player.vec.x - entityplayer.vec.x,  2) + pow(player.vec.z - entityplayer.vec.z ,2));
	float y = player.vec.y - entityplayer.vec.y;
	float c = -1;
	
	return c * atan(y / xz) * 180 / 3.141592;
}

void setAim(Player player,float x,float y)
{
	
	float x_angle = x;
	float y_angle = y;

	
	

    WriteProcessMemory(hTargetProc, LPVOID(player.base + pit_offset), &x_angle, sizeof(x_angle), 0); //��ġ
    WriteProcessMemory(hTargetProc, LPVOID(player.base + yaw_offset), &y_angle, sizeof(y_angle), 0);// ��

     
}

float getaimerr(Player p,float x1, float y1)
{
	return pow((p.vec.x - x1), 2) + pow((p.vec.y - y1), 2);
}

float GetDistance(Player p, Player e)
{
	return pow((p.vec.x - e.vec.x), 2) + pow((p.vec.z - e.vec.z), 2);
}


void WorldToScreen(Vec3 pos, Matrix viewmatrix, int SWidth, int SHeight) {
	// ��� ���ϱ� ���Ĵ�� �����ش�(ī�޶� ������ ��ǥ�� �������)
	float x_value = viewmatrix.matrix[0] * pos.x + viewmatrix.matrix[4] * pos.y + viewmatrix.matrix[8] * pos.z + viewmatrix.matrix[12];
	float y_value = viewmatrix.matrix[1] * pos.x + viewmatrix.matrix[5] * pos.y + viewmatrix.matrix[9] * pos.z + viewmatrix.matrix[13];
	// z��ǥ�� �ʿ�����ϴ�
	float w_value = viewmatrix.matrix[3] * pos.x + viewmatrix.matrix[7] * pos.y + viewmatrix.matrix[11] * pos.z + viewmatrix.matrix[15];

	// ����ȭ�� ��ġ ��ǥ(NDC)�� ��� ���� w��ǥ�� �����ݴϴ�(�����ȭ�� ��ǥ�� ��ȯ�ϱ� ���� ����)
	x_value /= w_value;
	y_value /= w_value;

	// y��ǥ�� -1�� ���ϴ� ������ ����� y��ǥ�� �Ʒ��� ���������� �������� �ϴµ�
	// ��� ������� y��ǥ�� �Ʒ��� ������ �������� ������ ���⼭ �������ִ� �̴ϴ�
	y_value *= -1;

	// ���߿� ����ϱ� ���� NDC�� �������ݴϴ�
	auto ndc_x = x_value;
	auto ndc_y = y_value;

	// �Ʒ����� ����Ʈ ��ȯ �����Դϴ�(����� ��ǥ�� ��ȯ)
	x_value /= 2;
	y_value /= 2;

	x_value *= SWidth;
	y_value *= SHeight;

	x_value += (ndc_x + SWidth / 2);
	y_value += (ndc_y + SHeight / 2);

	return;
}


Player entityplayer[30];

Matrix matrix;

int main()
{
	
	
	
	char targetProc[100] = "ac_client.exe";

	DWORD pid = GetProcId(targetProc);

	hTargetProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);

	if (hTargetProc == INVALID_HANDLE_VALUE)
	{
		printf("�ڵ� ��� ����");
	}

	player_base = 0x400000 + 0x18AC00;

	entity_base = 0x400000 + 0x191FCC;
	
	

	while (1) 
	{
		
		
		
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
			
			float x_angle = 0;
			float y_angle = 0;
			float min_err = 1000000;
			float err = 0;
			float min_x_angle = 0;
			float min_y_angle = 0;
			
			// ����ü ���� ����   // ���� ������ ������ �迭���·� for������ �о����
			Player player(hTargetProc);
			for (int i = 0; i < 30; i++) {
				entityplayer[i] = Player(hTargetProc, i);
			}
			// ������ �˰���

			for (int i = 0; i < 30; i++) {

				
				if (entityplayer[i].health  > 0) //���� ��������� ����
				{
					x_angle = Get2Ddegree(player, entityplayer[i]);
					y_angle = GetYawdegree(player, entityplayer[i]);
					err = GetDistance(player, entityplayer[i]); // �Ÿ� ���ϴ¹�
					if (min_err > err)
					{
						min_err = err;
						min_x_angle = x_angle;
						min_y_angle = y_angle;
					}
				}

				
			}
			
			setAim(player, min_x_angle, min_y_angle);
			printf("%f\n", player.pitch);
		}

		
		
	}
	return 0;
}

