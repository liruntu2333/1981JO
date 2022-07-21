#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "tree.h"
#include "bullet.h"
#include "score.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"

void CheckHit(void);

static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	 

HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 200, 200, 6.4f, 6.4f);

	InitPlayer();

	InitEnemy();

	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	InitTree();

	InitBullet();

	InitScore();

	InitParticle();

	PlaySound(SOUND_LABEL_BGM_sample001);

	return S_OK;
}

void UninitGame(void)
{
	UninitParticle();

	UninitScore();

	UninitBullet();

	UninitTree();

	UninitMeshWall();

	UninitMeshField();

	UninitEnemy();

	UninitPlayer();
}

void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}

#endif

	if (g_bPause == FALSE)
		return;

	UpdateMeshField();

	UpdatePlayer();

	UpdateEnemy();

	UpdateMeshWall();

	UpdateTree();

	UpdateBullet();

	CheckHit();

	UpdateScore();
}

void DrawGame0(void)
{
	DrawMeshField();

	DrawEnemy();

	DrawPlayer();

	DrawBullet();

	DrawMeshWall();

	DrawTree();

	SetDepthEnable(FALSE);

	SetLightEnable(FALSE);

	DrawScore();

	SetLightEnable(TRUE);

	SetDepthEnable(TRUE);
}

void DrawGame(void)
{
	XMFLOAT3 pos;

#ifdef _DEBUG
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	pos = GetPlayer()->pos;
	SetCameraAT(pos);
	SetCamera();

	switch (g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;
	}
}

void CheckHit(void)
{
	ENEMY* enemy = GetEnemy();		 
	PLAYER* player = GetPlayer();	 
	BULLET* bullet = GetBullet();	 

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE)
			continue;

		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
		{
			enemy[i].use = FALSE;

			AddScore(100);
		}
	}

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (bullet[i].use == FALSE)
			continue;

		for (int j = 0; j < MAX_ENEMY; j++)
		{
			if (enemy[j].use == FALSE)
				continue;

			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
			{
				bullet[i].use = FALSE;

				enemy[j].use = FALSE;

				AddScore(10);
			}
		}
	}

	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE) continue;
		enemy_count++;
	}

	if (enemy_count == 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}
}