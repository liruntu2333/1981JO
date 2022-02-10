////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightforshadow.h"

#pragma comment (lib, "d3dx9.lib")

////////////
// GLOBAL //
////////////
//static D3DXVECTOR4 g_ambientColor;
//static D3DXVECTOR4 g_diffuseColor;
static D3DXVECTOR3 g_position;
static D3DXVECTOR3 g_lookAt;
static D3DXMATRIX g_viewMatrix;
static D3DXMATRIX g_projectionMatrix;

//void SetSLAmbientColor(float red, float green, float blue, float alpha)
//{
//	g_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
//	return;
//}
//
//
//void SetSLDiffuseColor(float red, float green, float blue, float alpha)
//{
//	g_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
//	return;
//}

void SetSLPosition(float x, float y, float z)
{
	g_position = D3DXVECTOR3(x, y, z);
	return;
}

void SetSLLookAt(float x, float y, float z)
{
	g_lookAt.x = x;
	g_lookAt.y = y;
	g_lookAt.z = z;
	return;
}

//D3DXVECTOR4 GetSLAmbientColor()
//{
//	return g_ambientColor;
//}
//
//
//D3DXVECTOR4 GetSLDiffuseColor()
//{
//	return g_diffuseColor;
//}

D3DXVECTOR3 GetSLPosition()
{
	return g_position;
}

void GenerateSLViewMatrix()
{
	D3DXVECTOR3 up;

	// Setup the vector that points x+.
	up.x = 1.0f;
	up.y = 0.0f;
	up.z = 0.0f;

	// Create the view matrix from the three vectors.
	D3DXMatrixLookAtLH(&g_viewMatrix, &g_position, &g_lookAt, &up);

	return;
}

void GenerateSLProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView, screenAspect;

	// Setup field of view and screen aspect for a square light source.
	fieldOfView = (float)D3DX_PI / 2.0f;
	screenAspect = 1.0f;

	// Create the projection matrix for the light.
	D3DXMatrixPerspectiveFovLH(&g_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	return;
}

void GetSLViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = g_viewMatrix;
	return;
}

void GetSLProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = g_projectionMatrix;
	return;
}