//=============================================================================
//
// レンダリング処理 [renderer.h]
// Created by Li Runtu 2022 liruntu2333@gmail.com
//
//=============================================================================
#pragma once

//
// Include
//

//*********************************************************
// MACROS
//*********************************************************
#define LIGHT_MAX		(5)

enum LIGHT_TYPE
{
	LIGHT_TYPE_NONE,		//ライト無し
	LIGHT_TYPE_DIRECTIONAL,	//ディレクショナルライト
	LIGHT_TYPE_POINT,		//ポイントライト

	LIGHT_TYPE_NUM
};

enum BLEND_MODE
{
	BLEND_MODE_NONE,		//ブレンド無し
	BLEND_MODE_ALPHABLEND,	//αブレンド
	BLEND_MODE_ADD,			//加算ブレンド
	BLEND_MODE_SUBTRACT,	//減算ブレンド

	BLEDD_MODE_NUM
};

enum RASTERIZE_STATE
{
	CULL_MODE_NONE,			//カリング無し
	CULL_MODE_FRONT,		//表のポリゴンを描画しない(CW)
	CULL_MODE_BACK,			//裏のポリゴンを描画しない(CCW)
	CULL_MODE_BACK_DEPTH,

	CULL_MODE_NUM
};

//*********************************************************
// STRUCT
//*********************************************************

// 頂点STRUCT
struct VERTEX_3D
{
	XMFLOAT3	Position;
	XMFLOAT3	Normal;
	XMFLOAT4	Diffuse;
	XMFLOAT2	TexCoord;
};

// Material STRUCT
struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
};

// ライトSTRUCT
struct LIGHT {
	XMFLOAT3	Direction;	// ライトの方向
	XMFLOAT3	Position;	// ライトの位置
	XMFLOAT4	Diffuse;	// 拡散光の色
	XMFLOAT4	Ambient;	// 環境光の色
	float		Attenuation;// 減衰率
	int			Type;		// ライト種別・有効フラグ
	int			Enable;		// ライト種別・有効フラグ
};

// フォグSTRUCT
struct FOG {
	float		FogStart;	// フォグの開始距離
	float		FogEnd;		// フォグの最大距離
	XMFLOAT4	FogColor;	// フォグの色
};

struct LIGHT2 {
	XMFLOAT3	lightPosition;
	float		padding;
};

//*****************************************************************************
// Prototype declaration
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void UninitRenderer(void);

void Clear(void);
void Present(void);

ID3D11Device* GetDevice(void);
ID3D11DeviceContext* GetDeviceContext(void);

void SetDepthEnable(BOOL Enable);
void SetBlendState(BLEND_MODE bm);
void SetRasterizeState(RASTERIZE_STATE cm);
void SetAlphaTestEnable(BOOL flag);

void SetWorldViewProjection2D(void);
void SetWorldMatrix(XMMATRIX* WorldMatrix);
void SetViewMatrix(XMMATRIX* ViewMatrix);
void SetProjectionMatrix(XMMATRIX* ProjectionMatrix);
void SetLightViewMatrix(XMMATRIX* LightViewMatrix);
void SetLightProjMatrix(XMMATRIX* LightProjectionMatrix);

void SetMaterial(MATERIAL material);

void SetLightEnable(BOOL flag);
void SetLight(int index, LIGHT* light);
void SetShadowLight(LIGHT2* light);

void SetFogEnable(BOOL flag);
void SetFog(FOG* fog);

void DebugTextOut(char* text, int x, int y);

void SetFuchi(int flag);
void SetShaderCamera(XMFLOAT3 pos);

void ResetRenderer(void);