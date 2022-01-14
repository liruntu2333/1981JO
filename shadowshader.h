////////////////////////////////////////////////////////////////////////////////
// Filename: shadowshader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SHADOWSHADER_H_
#define _SHADOWSHADER_H_
#pragma warning(disable:4005)

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

///////////////////////////
// PROTOTYPE DECLARATION //
///////////////////////////
bool InitShadowShader(ID3D11Device*, HWND);
void ShutdownSS();
bool RenderShadow(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
	ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);

#endif