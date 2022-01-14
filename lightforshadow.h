////////////////////////////////////////////////////////////////////////////////
// Filename: lightforshadow.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTFORSHADOW_H_
#define _LIGHTFORSHADOW_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>

///////////////////////////
// PROTOTYPE DECLARATION //
///////////////////////////
//void SetSLAmbientColor(float, float, float, float);
//void SetSLDiffuseColor(float, float, float, float);
void SetSLPosition(float, float, float);
void SetSLLookAt(float, float, float);

//D3DXVECTOR4 GetSLAmbientColor();
//D3DXVECTOR4 GetSLDiffuseColor();
D3DXVECTOR3 GetSLPosition();

void GenerateSLViewMatrix();
void GenerateSLProjectionMatrix(float, float);

void GetSLViewMatrix(D3DXMATRIX&);
void GetSLProjectionMatrix(D3DXMATRIX&);

#endif