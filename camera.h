#pragma once

#include "renderer.h"

struct CAMERA
{
	XMFLOAT4X4			mtxView;
	XMFLOAT4X4			mtxInvView;
	XMFLOAT4X4			mtxProjection;

	XMFLOAT3			pos;
	XMFLOAT3			at;
	XMFLOAT3			up;
	XMFLOAT3			rot;

	float				len;
};

enum {
	TYPE_FULL_SCREEN,
	TYPE_LEFT_HALF_SCREEN,
	TYPE_RIGHT_HALF_SCREEN,
	TYPE_UP_HALF_SCREEN,
	TYPE_DOWN_HALF_SCREEN,
	TYPE_NONE,
};

void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);

CAMERA* GetCamera(void);

void SetViewPort(int type);
int GetViewPortType(void);

void SetCameraAT(XMFLOAT3 pos);
