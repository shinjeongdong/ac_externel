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

	Player(HANDLE phandle) : base(0), health(0), e_base(0), vec{ 0.0f, 0.0f, 0.0f }, pitch(0.0f), yaw(0.0f)
	{
		ReadProcessMemory(phandle, (LPCVOID)player_base, &base, sizeof(base), 0);
		ReadProcessMemory(phandle, (LPCVOID)(base + x_offset), &vec.x, sizeof(vec.x), 0);
		ReadProcessMemory(phandle, (LPCVOID)(base + z_offset), &vec.z, sizeof(vec.y), 0);
		ReadProcessMemory(phandle, (LPCVOID)(base + y_offset), &vec.y, sizeof(vec.z), 0);
		ReadProcessMemory(phandle, (LPCVOID)(base + pit_offset), &pitch, sizeof(pitch), 0);
		ReadProcessMemory(phandle, (LPCVOID)(base + yaw_offset), &yaw, sizeof(yaw), 0);

	}

	Player(HANDLE phandle, int i) : base(0), health(0), e_base(0), vec{ 0.0f, 0.0f, 0.0f }, pitch(0.0f), yaw(0.0f)
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


struct spos
{
	float x, y;
}screen;

spos WorldToScreen(Player entityplayer, Matrix viewmatrix, int SWidth, int SHeight) {
	// 행렬 곱하기 공식대로 곱해준다(카메라 관점의 좌표를 얻기위함)
	float x_value = viewmatrix.matrix[0] * entityplayer.vec.x + viewmatrix.matrix[4] * entityplayer.vec.z + viewmatrix.matrix[8] * entityplayer.vec.y + viewmatrix.matrix[12];
	float y_value = viewmatrix.matrix[1] * entityplayer.vec.x + viewmatrix.matrix[5] * entityplayer.vec.z + viewmatrix.matrix[9] * entityplayer.vec.y + viewmatrix.matrix[13];
	// z좌표는 필요없습니다
	float w_value = viewmatrix.matrix[3] * entityplayer.vec.x + viewmatrix.matrix[7] * entityplayer.vec.z + viewmatrix.matrix[11] * entityplayer.vec.y + viewmatrix.matrix[15];
	printf("%f\n", w_value);

	printf("\n");
	if (w_value < 0 || entityplayer.health <= 0)
	{
		return screen;
	}

	x_value /= w_value;
	y_value /= w_value;
	y_value *= -1;

	x_value = 1280 * (x_value + 1) / 2;
	y_value = 720 * (y_value + 1) / 2;

	screen.x = x_value;
	screen.y = y_value;

	
	
	return screen;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

Player entityplayer[30];

Matrix viewmatrix;

TCHAR szClassName[] = TEXT("MyFirstProgram");

HWND hWnd;

void esp(HDC hdc);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	
	

	FILE* pFile = nullptr;
	if (AllocConsole()) {
		freopen_s(&pFile, "CONIN$", "rb", stdin);
		freopen_s(&pFile, "CONOUT$", "wb", stdout);
		freopen_s(&pFile, "CONOUT$", "wb", stderr);
	}
	else
	{
		return 0;
	}
	
	
	char targetProc[20] = "ac_client.exe";

	DWORD pid = GetProcId(targetProc);

	hTargetProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);

	if (hTargetProc == INVALID_HANDLE_VALUE)
	{
		printf("핸들 얻기 실패");
	}

	player_base = 0x400000 + 0x18AC00;

	entity_base = 0x400000 + 0x191FCC;

	hWnd;
	MSG msg;
	WNDCLASSEX wc;

	

	// 윈도우 클래스 등록
	wc.hInstance = hInstance;
	wc.lpszClassName = szClassName; //클래스 이름 
	wc.lpszMenuName = NULL; //프로그램의 메뉴 사용 안함
	wc.lpfnWndProc = WndProc; //호출하는 주체가 윈도우 
	wc.style = CS_DBLCLKS; //동작의 특성 지정. 더블클릭 잡아냄
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); //아이콘 핸들. 아이콘 지정 
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); //작은아이콘 핸들. 작게 보일 아이콘 지정
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //커서 핸들. 기본 마우스 커서 사용 
	wc.cbClsExtra = NULL; //윈도우 클래스 뒤에 여유 바이트 없음 
	wc.cbWndExtra = NULL; //윈도우 인스턴스 또는 구조체 

	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	
	if (!RegisterClassEx(&wc))
	{
		printf("레지스터 실패");
	}

	// 윈도우 생성후 핸들 hWnd에 반환

	hWnd = CreateWindowEx(WS_EX_TOPMOST, szClassName, 0, WS_OVERLAPPEDWINDOW, 100, 90, 1280, 720, NULL, NULL, hInstance, NULL);

	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	
	// 윈도우 표시
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	
	while (GetMessage(&msg, 0, 0, 0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	


	
	
	return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static Player player;
	char string[100];
	switch (message) {
	case WM_PAINT:
	{

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		esp(hdc);
		
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_DESTROY: {
		MessageBox(hWnd, TEXT("destroy! "), TEXT("my program"), MB_OK | MB_ICONEXCLAMATION);
		PostQuitMessage(0);
		break;
	}
	case WM_CLOSE: {
		MessageBox(hWnd, TEXT("꺼짐"), TEXT("my program"), MB_OK | MB_ICONEXCLAMATION);
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}



void esp(HDC hdc)
{

	for (int i = 0; i < 10; i++) {
		

		entityplayer[i] = Player(hTargetProc, i);
		if (entityplayer[i].health == 0)
		{
			continue;
		}
		ReadProcessMemory(hTargetProc, (LPCVOID)(0x400000 + 0x17DFD0), &viewmatrix, sizeof(Matrix), 0);
		spos sp = WorldToScreen(entityplayer[i], viewmatrix, 1280, 720);

		SetTextColor(hdc, RGB(0xFF, 0x00, 0x00));
		SetBkMode(hdc, TRANSPARENT);
		TextOutA(hdc, sp.x, sp.y, "ememy", 5);

	}
	return;
}