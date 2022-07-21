#include "model.h"
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"
#include "depthshader.h"

#define	MODEL_PLAYER		"data/MODEL/carrier.obj"			 
#define	MODEL_PLAYER_PARTS	"data/MODEL/frigate.obj"			 

#define	VALUE_MOVE			(2.0f)							 
#define	VALUE_ROTATE		(XM_PI * 0.02f)					 

#define PLAYER_SHADOW_SIZE	(1.0f)							 
#define PLAYER_OFFSET_Y		(10.0f)							 

#define PLAYER_PARTS_MAX	(2)								 

static PLAYER		g_Player;						 

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		 

static BOOL			g_Load = FALSE;

static INTERPOLATION_DATA move_tbl_right[] = {	    
	{ XMFLOAT3(20.0f, 8.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(20.0f, 4.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 4, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(20.0f, 8.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_left[] = {	    
	{ XMFLOAT3(-20.0f, 8.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-20.0f, 8.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, XM_PI / 2),    XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(-20.0f, 8.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	g_Player.pos = { 130.0f, PLAYER_OFFSET_Y, 100.0f };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };

	g_Player.spd = 0.0f;			 
	g_Player.size = PLAYER_SIZE;	 

	g_Player.use = TRUE;

	g_Player.parent = NULL;			 

	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Parts[i].parent = &g_Player;		  
		g_Parts[i].tbl_adr = NULL;		 
		g_Parts[i].move_time = 0.0f;	 
		g_Parts[i].tbl_size = 0;		 

		g_Parts[i].load = 0;
	}

	g_Parts[0].use = TRUE;
	g_Parts[0].parent = &g_Player;		 
	g_Parts[0].tbl_adr = move_tbl_right;	 
	g_Parts[0].tbl_size = sizeof(move_tbl_right) / sizeof(INTERPOLATION_DATA);		 
	g_Parts[0].load = 1;
	LoadModel(MODEL_PLAYER_PARTS, &g_Parts[0].model);

	g_Parts[1].use = TRUE;
	g_Parts[1].parent = &g_Player;		 
	g_Parts[1].tbl_adr = move_tbl_left;	 
	g_Parts[1].tbl_size = sizeof(move_tbl_left) / sizeof(INTERPOLATION_DATA);		 
	g_Parts[1].load = 1;
	LoadModel(MODEL_PLAYER_PARTS, &g_Parts[1].model);

	g_Load = TRUE;
	return S_OK;
}

void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	g_Load = FALSE;
}

void UpdatePlayer(void)
{
	CAMERA* cam = GetCamera();

	if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_LEFT))
	{	 
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, BUTTON_RIGHT))
	{	 
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = -XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_W) || IsButtonPressed(0, BUTTON_UP))
	{	 
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = XM_PI;
	}
	if (GetKeyboardPress(DIK_S) || IsButtonPressed(0, BUTTON_DOWN))
	{	 
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = 0.0f;
	}

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = g_Player.dir = 0.0f;
		g_Player.spd = 0.0f;
	}
#endif

	if (g_Player.spd > 0.0f)
	{
		g_Player.rot.y = g_Player.dir + cam->rot.y;

		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}

	XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				 
	XMFLOAT3 hitPosition;								 
	hitPosition.y = g_Player.pos.y - PLAYER_OFFSET_Y;	 
	bool ans = RayHitField(g_Player.pos, &hitPosition, &normal);
	g_Player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;
	
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		SetBullet(g_Player.pos, g_Player.rot);
	}

	g_Player.spd *= 0.5f;

	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if ((g_Parts[i].use == TRUE) && (g_Parts[i].tbl_adr != NULL))
		{
			int		index = (int)g_Parts[i].move_time;
			float	time = g_Parts[i].move_time - index;
			int		size = g_Parts[i].tbl_size;

			float dt = 1.0f / g_Parts[i].tbl_adr[index].frame;	 
			g_Parts[i].move_time += dt;					 

			if (index > (size - 2))	 
			{
				g_Parts[i].move_time = 0.0f;
				index = 0;
			}

			XMVECTOR p1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].pos);	 
			XMVECTOR p0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].pos);	 
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Parts[i].pos, p0 + vec * time);

			XMVECTOR r1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].rot);	 
			XMVECTOR r0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].rot);	 
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Parts[i].rot, r0 + rot * time);

			XMVECTOR s1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].scl);	 
			XMVECTOR s0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].scl);	 
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Parts[i].scl, s0 + scl * time);
		}
	}

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;

	g_Player.upVector = normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.upVector));

	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	angle = asinf(len);
	quat = XMQuaternionRotationNormal(nvx, angle);

	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.quaternion), quat, 0.05f);

	XMStoreFloat4(&g_Player.quaternion, quat);

#ifdef _DEBUG	 
	PrintDebugProc("Player:Å™ Å® Å´ Å©Å@Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

void DrawPlayer(void)
{
	SetRasterizeState(CULL_MODE_NONE);

	auto mtxWorld = XMLoadFloat4x4(&g_Player.mtxWorld);

	SetWorldMatrix(&mtxWorld);

	SetFuchi(1);
	DrawModel(&g_Player.model);

	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].use == FALSE) continue;

		auto mtxWorld = XMLoadFloat4x4(&g_Parts[i].mtxWorld);

		SetWorldMatrix(&mtxWorld);

		SetFuchi(1);
		DrawModel(&g_Parts[i].model);
	}

	SetFuchi(0);

	SetRasterizeState(CULL_MODE_BACK);
}

bool RenderPlayerWithDepthShader(D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix)
{
	XMMATRIX mtxWorld = XMMatrixIdentity();

	XMMATRIX mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	XMMATRIX mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

	if (!RenderModelToTexture(&g_Player.model,
		xmmatrix2d3dmatrix(mtxWorld),
		lightViewMatrix, lightProjectionMatrix))
		return false;

	for (size_t i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		XMMATRIX mtxWorld = XMMatrixIdentity();

		XMMATRIX mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		XMMATRIX mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);


		if (g_Parts[i].parent != NULL)	 
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		if (g_Parts[i].use == FALSE) continue;

		if (!RenderModelToTexture(&g_Parts[i].model,
			xmmatrix2d3dmatrix(mtxWorld),
			lightViewMatrix, lightProjectionMatrix))
			return false;
	}
	return true;
}

PLAYER* GetPlayer(void)
{
	return &g_Player;
}