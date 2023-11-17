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
		printf("hProc 모듈 얻기 실패");
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
		printf("hModSnap 모듈 얻기 실패");
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



float Get2Ddegree(Player player, Player entityplayer) //위에서 봤을때(2d)상태의 각도 구하기
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

float GetYawdegree(Player player, Player entityplayer) // 옆에서 봤을때(높낮이)상태의 각도 구하기
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

	
	

    WriteProcessMemory(hTargetProc, LPVOID(player.base + pit_offset), &x_angle, sizeof(x_angle), 0); //피치
    WriteProcessMemory(hTargetProc, LPVOID(player.base + yaw_offset), &y_angle, sizeof(y_angle), 0);// 요

     
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
	// 행렬 곱하기 공식대로 곱해준다(카메라 관점의 좌표를 얻기위함)
	float x_value = viewmatrix.matrix[0] * pos.x + viewmatrix.matrix[4] * pos.y + viewmatrix.matrix[8] * pos.z + viewmatrix.matrix[12];
	float y_value = viewmatrix.matrix[1] * pos.x + viewmatrix.matrix[5] * pos.y + viewmatrix.matrix[9] * pos.z + viewmatrix.matrix[13];
	// z좌표는 필요없습니다
	float w_value = viewmatrix.matrix[3] * pos.x + viewmatrix.matrix[7] * pos.y + viewmatrix.matrix[11] * pos.z + viewmatrix.matrix[15];

	// 정규화된 장치 좌표(NDC)를 얻기 위해 w좌표를 나눠줍니다(모니터화면 좌표로 변환하기 위한 과정)
	x_value /= w_value;
	y_value /= w_value;

	// y좌표에 -1을 곱하는 이유는 모니터 y좌표는 아래로 내려갈수록 높아져야 하는데
	// 계산 결과에서 y좌표는 아래로 갈수록 낮아지기 때문에 여기서 보정해주는 겁니다
	y_value *= -1;

	// 나중에 계산하기 위해 NDC를 저장해줍니다
	auto ndc_x = x_value;
	auto ndc_y = y_value;

	// 아래쪽은 뷰포트 변환 공식입니다(모니터 좌표로 변환)
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
		printf("핸들 얻기 실패");
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
			
			// 구조체 정보 갱신   // 여러 적들의 정보를 배열형태로 for문으로 읽어오자
			Player player(hTargetProc);
			for (int i = 0; i < 30; i++) {
				entityplayer[i] = Player(hTargetProc, i);
			}
			// 에임핵 알고리즘

			for (int i = 0; i < 30; i++) {

				
				if (entityplayer[i].health  > 0) //적이 살아있으면 실행
				{
					x_angle = Get2Ddegree(player, entityplayer[i]);
					y_angle = GetYawdegree(player, entityplayer[i]);
					err = GetDistance(player, entityplayer[i]); // 거리 구하는법
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

