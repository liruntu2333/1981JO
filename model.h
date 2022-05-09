//=============================================================================
// [model.h]
// Author : LiRuntu
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "depthshader.h"

//*********************************************************
// STRUCT
//*********************************************************

// Material STRUCT

#define MODEL_MAX_MATERIAL		(16)		// １モデルのMaxMaterial 数

struct DX11_MODEL_MATERIAL
{
	MATERIAL					Material;
	ID3D11ShaderResourceView* Texture;
};

// 描画サブセットSTRUCT
struct DX11_SUBSET
{
	unsigned short	StartIndex;
	unsigned short	IndexNum;
	DX11_MODEL_MATERIAL	Material;
};

struct DX11_MODEL
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	DX11_SUBSET* SubsetArray;
	unsigned short	SubsetNum;
};

//*****************************************************************************
// Prototype declaration
//*****************************************************************************
void LoadModel(char* FileName, DX11_MODEL* Model);
void UnloadModel(DX11_MODEL* Model);
void DrawModel(DX11_MODEL* Model);
bool RenderModelToTexture(DX11_MODEL* Model, D3DXMATRIX mtxWorld, D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix);

// モデルのMaterial のディフューズを取得する。Max16個分にしてある
void GetModelDiffuse(DX11_MODEL* Model, XMFLOAT4* diffuse);

// モデルの指定Material のディフューズをセットする。
void SetModelDiffuse(DX11_MODEL* Model, int mno, XMFLOAT4 diffuse);
