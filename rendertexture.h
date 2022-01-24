////////////////////////////////////////////////////////////////////////////////
// Filename: rendertexture.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#pragma warning(disable:4005)
//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>

///////////////////////////
// PROTOTYPE DECLARATION //
///////////////////////////
bool InitRenderTex(ID3D11Device*, int, int, float, float);
void ShutdownRenderTex();

void SetRTRenderTarget(ID3D11DeviceContext*);
void ClearRTRenderTarget(ID3D11DeviceContext*, float, float, float, float);
ID3D11ShaderResourceView* GetRTShaderResourceView();
void GetRTProjectionMatrix(D3DXMATRIX&);
void GetRTOrthoMatrix(D3DXMATRIX&);

#endif