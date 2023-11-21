#pragma once
#include <Windows.h>
#include "main.h"

struct Vec3
{
	float x, z, y;
};

struct Vec2
{
	float pitch, yaw;
};

extern struct spos
{
	float x, y;
}screen;

typedef struct Matrix {
	float matrix[16];
};

class local
{
public:
	uintptr_t player_base = 0;
	////////
	int health = 0;
	Vec3 pos3 = {0,0,0};
	Vec2 viewangle = {0,0};
	Matrix viewmatrix = { 0,};

public:
	BOOL GetHealth();
	BOOL GetPos();
	BOOL GetViewangle();
	BOOL GetViewMartix();
};


class entity
{
public:
	uintptr_t entity_base = 0;
	//////////////
	int health = 0;
	int alive = 0;
	Vec3 pos3 = {0,0,0};
	Vec2 viewangle;

public:
	BOOL GetHealth();
	BOOL GetPos();


};
