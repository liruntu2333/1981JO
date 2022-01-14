////////////////////////////////////////////////////////////////////////////////
// Filename: depthshader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DEPTHSHADERCLASS_H_
#define _DEPTHSHADERCLASS_H_
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
bool InitDepthShader(ID3D11Device*, HWND);
void ShutdownDS();
bool RenderDepthShader(ID3D11DeviceContext*, int, int startIndexLoc, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

#endif