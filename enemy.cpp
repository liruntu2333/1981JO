#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "depthshader.h"
#include "meshfield.h"

#define	MODEL_ENEMY			"data/MODEL/cruiser.obj"		 

#define	VALUE_MOVE			(5.0f)						 
#define	VALUE_ROTATE		(XM_PI * 0.02f)				 

#define ENEMY_SHADOW_SIZE	(0.4f)						 
#define ENEMY_OFFSET_Y		(10.0f)						 

static ENEMY			g_Enemy[MAX_ENEMY];				 

static BOOL				g_Load = FALSE;

static INTERPOLATION_DATA move_tbl[] = {	    
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, XM_PI, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, XM_2PI, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2 },
};

HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 50.0f * (i % 2 ? 1.0f : -1.0f));
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = 0.0f;			 
		g_Enemy[i].size = ENEMY_SIZE;	 

		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);

		g_Enemy[i].move_time = 0.0f;	 
		g_Enemy[i].tbl_adr = NULL;		 
		g_Enemy[i].tbl_size = 0;		 

		g_Enemy[i].use = TRUE;			 
	}

	GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

	XMFLOAT3 pos = g_Enemy[MAX_ENEMY - 1].pos;
	pos.y -= (ENEMY_OFFSET_Y - 0.1f);

	g_Enemy[MAX_ENEMY - 1].move_time = 0.0f;	 
	g_Enemy[MAX_ENEMY - 1].tbl_adr = NULL;		 
	g_Enemy[MAX_ENEMY - 1].tbl_size = 0;		 

	g_Enemy[MAX_ENEMY - 1].use = TRUE;			 

	g_Enemy[0].move_time = 0.0f;		 
	g_Enemy[0].tbl_adr = move_tbl;		 
	g_Enemy[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	 

	g_Load = TRUE;
	return S_OK;
}

void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

void UpdateEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)			 
		{									 
			if (g_Enemy[i].tbl_adr != NULL)	 
			{								 
				int		index = (int)g_Enemy[i].move_time;
				float	time = g_Enemy[i].move_time - index;
				int		size = g_Enemy[i].tbl_size;

				float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	 
				g_Enemy[i].move_time += dt;							 

				if (index > (size - 2))	 
				{
					g_Enemy[i].move_time = 0.0f;
					index = 0;
				}

				XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	 
				XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	 
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

				XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	 
				XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	 
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

				XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	 
				XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	 
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);
			}
			XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				 
			XMFLOAT3 hitPosition;								 
			hitPosition.y = g_Enemy[i].pos.y - ENEMY_OFFSET_Y;	 
			bool ans = RayHitField(g_Enemy[i].pos, &hitPosition, &normal);
			g_Enemy[i].pos.y = hitPosition.y + ENEMY_OFFSET_Y;

			XMVECTOR vx, nvx, up;
			XMVECTOR quat;
			float len, angle;

			g_Enemy[i].upVector = normal;
			up = { 0.0f, 1.0f, 0.0f, 0.0f };
			vx = XMVector3Cross(up, XMLoadFloat3(&g_Enemy[i].upVector));

			nvx = XMVector3Length(vx);
			XMStoreFloat(&len, nvx);
			nvx = XMVector3Normalize(vx);
			angle = asinf(len);
			quat = XMQuaternionRotationNormal(nvx, angle);

			quat = XMQuaternionSlerp(XMLoadFloat4(&g_Enemy[i].quaternion), quat, 0.05f);

			XMStoreFloat4(&g_Enemy[i].quaternion, quat);
		}
	}
}

void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	SetRasterizeState(CULL_MODE_NONE);

	SetFuchi(1);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		mtxWorld = XMMatrixIdentity();

		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Enemy[i].quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		if (i == MAX_ENEMY - 1) SetFuchi(1);
		DrawModel(&g_Enemy[i].model);
	}
	SetFuchi(0);
	SetRasterizeState(CULL_MODE_BACK);
}

ENEMY* GetEnemy()
{
	return &g_Enemy[0];
}

bool RenderEnemyWithDepthShader(D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix)
{
	for (size_t i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		XMMATRIX mtxWorld = XMMatrixIdentity();

		XMMATRIX mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Enemy[i].quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		XMMATRIX mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (!RenderModelToTexture(&g_Enemy[i].model,
			xmmatrix2d3dmatrix(mtxWorld),
			lightViewMatrix, lightProjectionMatrix))
			return false;
	}
	return true;
}