#ifndef _LIGHTFORSHADOW_H_
#define _LIGHTFORSHADOW_H_

#pragma warning(disable:4005)
#include <d3dx10math.h>

void SetSLPosition(float, float, float);
void SetSLLookAt(float, float, float);

D3DXVECTOR3 GetSLPosition();

void GenerateSLViewMatrix();
void GenerateSLProjectionMatrix(float, float);

void GetSLViewMatrix(D3DXMATRIX&);
void GetSLProjectionMatrix(D3DXMATRIX&);

#endif