//=============================================================================
//
// メイン処理 [main.h]
// Author :
//
//=============================================================================
#pragma once

#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf のwarning防止
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <d3dx10math.h>

// 本来はヘッダに書かない方が良い
using namespace DirectX;

#define DIRECTINPUT_VERSION 0x0800		// 警告対処
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)

//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define SCREEN_WIDTH	(1280)			// ウインドウの幅
#define SCREEN_HEIGHT	(720)			// ウインドウの高さ
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)	// ウインドウの中心Ｘ座標
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	// ウインドウの中心Ｙ座標

#define	MAP_W			(1280.0f)
#define	MAP_H			(1280.0f)
#define	MAP_TOP			(MAP_H/2)
#define	MAP_DOWN		(-MAP_H/2)
#define	MAP_LEFT		(-MAP_W/2)
#define	MAP_RIGHT		(MAP_W/2)

// 補間用のデータ構造体を定義
struct INTERPOLATION_DATA
{
	XMFLOAT3	pos;		// 頂点座標
	XMFLOAT3	rot;		// 回転
	XMFLOAT3	scl;		// 拡大縮小
	float		frame;		// 実行フレーム数 ( dt = 1.0f/frame )
};

enum
{
	MODE_TITLE = 0,			// タイトル画面
	MODE_TUTORIAL,			// ゲーム説明画面
	MODE_GAME,				// ゲーム画面
	MODE_RESULT,			// リザルト画面
	MODE_MAX
};

//*****************************************************************************
// プロトタイプ宣言
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