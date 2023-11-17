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


struct spos
{
	float x, y;
}screen;

spos WorldToScreen(Player entityplayer, Matrix viewmatrix, int SWidth, int SHeight) {
	// ��� ���ϱ� ���Ĵ�� �����ش�(ī�޶� ������ ��ǥ�� �������)
	float x_value = viewmatrix.matrix[0] * entityplayer.vec.x + viewmatrix.matrix[4] * entityplayer.vec.z + viewmatrix.matrix[8] * entityplayer.vec.y + viewmatrix.matrix[12];
	float y_value = viewmatrix.matrix[1] * entityplayer.vec.x + viewmatrix.matrix[5] * entityplayer.vec.z + viewmatrix.matrix[9] * entityplayer.vec.y + viewmatrix.matrix[13];
	// z��ǥ�� �ʿ�����ϴ�
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
		printf("�ڵ� ��� ����");
	}

	player_base = 0x400000 + 0x18AC00;

	entity_base = 0x400000 + 0x191FCC;

	hWnd;
	MSG msg;
	WNDCLASSEX wc;

	

	// ������ Ŭ���� ���
	wc.hInstance = hInstance;
	wc.lpszClassName = szClassName; //Ŭ���� �̸� 
	wc.lpszMenuName = NULL; //���α׷��� �޴� ��� ����
	wc.lpfnWndProc = WndProc; //ȣ���ϴ� ��ü�� ������ 
	wc.style = CS_DBLCLKS; //������ Ư�� ����. ����Ŭ�� ��Ƴ�
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); //������ �ڵ�. ������ ���� 
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); //���������� �ڵ�. �۰� ���� ������ ����
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //Ŀ�� �ڵ�. �⺻ ���콺 Ŀ�� ��� 
	wc.cbClsExtra = NULL; //������ Ŭ���� �ڿ� ���� ����Ʈ ���� 
	wc.cbWndExtra = NULL; //������ �ν��Ͻ� �Ǵ� ����ü 

	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	
	if (!RegisterClassEx(&wc))
	{
		printf("�������� ����");
	}

	// ������ ������ �ڵ� hWnd�� ��ȯ

	hWnd = CreateWindowEx(WS_EX_TOPMOST, szClassName, 0, WS_OVERLAPPEDWINDOW, 100, 90, 1280, 720, NULL, NULL, hInstance, NULL);

	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	
	// ������ ǥ��
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
		MessageBox(hWnd, TEXT("����"), TEXT("my program"), MB_OK | MB_ICONEXCLAMATION);
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