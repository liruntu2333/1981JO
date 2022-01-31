//=============================================================================
//
// メッシュ壁の処理 [meshwall.h]
// Created by Li Runtu 2022 liruntu2333@gmail.com
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMeshWall(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT4 col,
						int nNumBlockX, int nNumBlockY, float fSizeBlockX, float fSizeBlockY);
void UninitMeshWall(void);
void UpdateMeshWall(void);
void DrawMeshWall(void);

