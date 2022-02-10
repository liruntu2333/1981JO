//=============================================================================
//
// 木処理 [tree.h]
// Created by Li Runtu 2022 liruntu2333@gmail.com
//
//=============================================================================
#pragma once

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTree(void);
void UninitTree(void);
void UpdateTree(void);
void DrawTree(void);

int SetTree(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);
