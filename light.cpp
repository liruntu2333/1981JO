//=============================================================================
//
// ライト処理 [light.cpp]
// Created by Li Runtu 2022 liruntu2333@gmail.com
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "lightforshadow.h"

//*****************************************************************************
// MACROS
//*****************************************************************************

//*****************************************************************************
// Prototype declaration
//*****************************************************************************

//*****************************************************************************
// GLOBALS
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

//=============================================================================
// 初期化処理
//=============================================================================
void InitLight(void)
{
	//ライト初期化
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position = XMFLOAT3(0.0f, 10.0f, 0.0f);
		g_Light[i].Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
		g_Light[i].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_Light[i].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.0f);
		g_Light[i].Attenuation = 100.0f;	// 減衰距離
		g_Light[i].Type = LIGHT_TYPE_POINT;	// ライトのタイプ
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}
	//g_Light[LIGHT_MAX - 1].Enable = TRUE;
	//SetLight(LIGHT_MAX - 1, &g_Light[LIGHT_MAX - 1]);

	// 並行光源の設定（世界を照らす光）
	g_Light[0].Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);		// 光の向き
	g_Light[0].Diffuse = XMFLOAT4(.7f, .7f, .7f, 1.0f);	// 光の色
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// 並行光源
	g_Light[0].Enable = TRUE;									// このライトをON
	SetLight(0, &g_Light[0]);									// これで設定している

	// フォグの初期化（霧の効果）
	g_Fog.FogStart = 100.0f;									// 視点からこの距離離れるとフォグがかかり始める
	g_Fog.FogEnd = 250.0f;									// ここまで離れるとフォグの色で見えなくなる
	g_Fog.FogColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);		// フォグの色
	SetFog(&g_Fog);
	SetFogEnable(FALSE);		// 他の場所もチェックする shadow
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateLight(void)
{
	//g_Light[LIGHT_MAX - 1].Position = GetPlayer()->pos;
	//g_Light[LIGHT_MAX - 1].Position.y += 20.0f;
	//SetLight(LIGHT_MAX - 1, &g_Light[LIGHT_MAX - 1]);
}

//=============================================================================
// ライトの設定
// Typeによってセットするメンバー変数が変わってくる
//=============================================================================
void SetLightData(int index, LIGHT* light)
{
	SetLight(index, light);
}

LIGHT* GetLightData(int index)
{
	return(&g_Light[index]);
}

//=============================================================================
// フォグの設定
//=============================================================================
void SetFogData(FOG* fog)
{
	SetFog(fog);
}