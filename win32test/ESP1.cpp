#include "ESP1.h"
#include <stdio.h>

spos screen;

spos ESP::WorldToScreen(entity entityplayer, local local, int SWidth, int SHeight) {
	// 행렬 곱하기 공식대로 곱해준다(카메라 관점의 좌표를 얻기위함)
	float x_value = local.viewmatrix.matrix[0] * entityplayer.pos3.x + local.viewmatrix.matrix[4] * entityplayer.pos3.z + local.viewmatrix.matrix[8] * entityplayer.pos3.y + local.viewmatrix.matrix[12];
	float y_value = local.viewmatrix.matrix[1] * entityplayer.pos3.x + local.viewmatrix.matrix[5] * entityplayer.pos3.z + local.viewmatrix.matrix[9] * entityplayer.pos3.y + local.viewmatrix.matrix[13];
	// z좌표는 필요없습니다
	float w_value =  local.viewmatrix.matrix[3] * entityplayer.pos3.x + local.viewmatrix.matrix[7] * entityplayer.pos3.z + local.viewmatrix.matrix[11] * entityplayer.pos3.y + local.viewmatrix.matrix[15];
	
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

	printf("%f\n", x_value);

	printf("\n");

	return screen;
}

