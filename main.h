//=============================================================================
//
// ���C������ [main.h]
// Author :
//
//=============================================================================
#pragma once

#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf ��warning�h�~
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <d3dx10math.h>

// �{���̓w�b�_�ɏ����Ȃ������ǂ�
using namespace DirectX;

#define DIRECTINPUT_VERSION 0x0800		// �x���Ώ�
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)

//*****************************************************************************
// ���C�u�����̃����N
//*****************************************************************************
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define SCREEN_WIDTH	(1280)			// �E�C���h�E�̕�
#define SCREEN_HEIGHT	(720)			// �E�C���h�E�̍���
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)	// �E�C���h�E�̒��S�w���W
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	// �E�C���h�E�̒��S�x���W

#define	MAP_W			(1280.0f)
#define	MAP_H			(1280.0f)
#define	MAP_TOP			(MAP_H/2)
#define	MAP_DOWN		(-MAP_H/2)
#define	MAP_LEFT		(-MAP_W/2)
#define	MAP_RIGHT		(MAP_W/2)

// ��ԗp�̃f�[�^�\���̂��`
struct INTERPOLATION_DATA
{
	XMFLOAT3	pos;		// ���_���W
	XMFLOAT3	rot;		// ��]
	XMFLOAT3	scl;		// �g��k��
	float		frame;		// ���s�t���[���� ( dt = 1.0f/frame )
};

enum
{
	MODE_TITLE = 0,			// �^�C�g�����
	MODE_TUTORIAL,			// �Q�[���������
	MODE_GAME,				// �Q�[�����
	MODE_RESULT,			// ���U���g���
	MODE_MAX
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
long GetMousePosX(void);
long GetMousePosY(void);
char* GetDebugStr(void);

void SetMode(int mode);
int GetMode(void);

inline XMMATRIX d3dmatrix2xmmatrix(D3DXMATRIX& M)
{
	return XMMATRIX{
		M._11, M._12, M._13, M._14,
		M._21, M._22, M._23, M._24,
		M._31, M._32, M._33, M._34,
		M._41, M._42, M._43, M._44, };
}

inline D3DXMATRIX xmmatrix2d3dmatrix(XMMATRIX& M)
{
	XMFLOAT4X4 tmp;
	XMStoreFloat4x4(&tmp, M);
	return D3DXMATRIX{
		tmp._11, tmp._12, tmp._13, tmp._14,
		tmp._21, tmp._22, tmp._23, tmp._24,
		tmp._31, tmp._32, tmp._33, tmp._34,
		tmp._41, tmp._42, tmp._43, tmp._44, };
}