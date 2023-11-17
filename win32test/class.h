#pragma once
#include <Windows.h>

struct Vec3
{
	float x, z, y;
};

struct Vec2
{
	float pitch, yaw;
};


class local
{
public:
	uintptr_t player_base = 0;
	////////
	int health = 0;
	Vec3 pos3;
	Vec2 viewangle;

public:
	BOOL GetHealth();
	BOOL GetPos();
	BOOL GetViewangle();
};


class entity
{
public:
	uintptr_t entity_base = 0;
	//////////////
	int health = 0;
	int alive = 0;
	Vec3 pos3;
	Vec2 viewangle;

public:
	BOOL GetHealth();
	BOOL GetPos();


};
