#pragma once

#define LIGHT_MAX		(5)

enum LIGHT_TYPE
{
	LIGHT_TYPE_NONE,		
	LIGHT_TYPE_DIRECTIONAL,	
	LIGHT_TYPE_POINT,		

	LIGHT_TYPE_NUM
};

enum BLEND_MODE
{
	BLEND_MODE_NONE,		
	BLEND_MODE_ALPHABLEND,	
	BLEND_MODE_ADD,			
	BLEND_MODE_SUBTRACT,	

	BLEDD_MODE_NUM
};

enum RASTERIZE_STATE
{
	CULL_MODE_NONE,			
	CULL_MODE_FRONT,		
	CULL_MODE_BACK,			
	CULL_MODE_BACK_DEPTH,

	CULL_MODE_NUM
};

struct VERTEX_3D
{
	XMFLOAT3	Position;
	XMFLOAT3	Normal;
	XMFLOAT4	Diffuse;
	XMFLOAT2	TexCoord;
};

struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
};

struct LIGHT {
	XMFLOAT3	Direction;	 
	XMFLOAT3	Position;	 
	XMFLOAT4	Diffuse;	 
	XMFLOAT4	Ambient;	 
	float		Attenuation; 
	int			Type;		 
	int			Enable;		 
};

struct FOG {
	float		FogStart;	 
	float		FogEnd;		 
	XMFLOAT4	FogColor;	 
};

struct LIGHT2 {
	XMFLOAT3	lightPosition;
	float		padding;
};

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