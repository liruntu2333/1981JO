#pragma once

HRESULT InitTree(void);
void UninitTree(void);
void UpdateTree(void);
void DrawTree(void);

int SetTree(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);
