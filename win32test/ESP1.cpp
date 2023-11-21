#include "ESP1.h"
#include <stdio.h>

spos screen;

spos ESP::WorldToScreen(entity entityplayer, local local, int SWidth, int SHeight) {
	// ��� ���ϱ� ���Ĵ�� �����ش�(ī�޶� ������ ��ǥ�� �������)
	float x_value = local.viewmatrix.matrix[0] * entityplayer.pos3.x + local.viewmatrix.matrix[4] * entityplayer.pos3.z + local.viewmatrix.matrix[8] * entityplayer.pos3.y + local.viewmatrix.matrix[12];
	float y_value = local.viewmatrix.matrix[1] * entityplayer.pos3.x + local.viewmatrix.matrix[5] * entityplayer.pos3.z + local.viewmatrix.matrix[9] * entityplayer.pos3.y + local.viewmatrix.matrix[13];
	// z��ǥ�� �ʿ�����ϴ�
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

