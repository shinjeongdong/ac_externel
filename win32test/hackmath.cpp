#include "hackmath.h"

float math::Get2Ddegree(Vec3 lpos3, Vec3 epos3)
{
	float x = lpos3.x - epos3.x;
	float z = lpos3.z - epos3.z;
	float c = 270;

	if (x < 0)
	{
		c = 90;
	}

	return c + atan(z / x) * 180 / 3.141592;
}


float math::GetYawdegree(Vec3 lpos3, Vec3 epos3)
{
	float xz = sqrt(pow(lpos3.x - epos3.x, 2) + pow(lpos3.z - epos3.z, 2));
	float y = lpos3.y - epos3.y;
	float c = -1;

	return c * atan(y / xz) * 180 / 3.141592;
}


float math::GetDistance(Vec3 lpos3, Vec3 epos3)
{
	return pow((lpos3.x - epos3.x), 2) + pow((lpos3.z - epos3.z), 2);
}