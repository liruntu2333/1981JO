//=============================================================================
//
// íeî≠éÀèàóù [bullet.h]
// Created by Li Runtu 2022 liruntu2333@gmail.com
//
//=============================================================================
#pragma once

//*****************************************************************************
// MACROS
//*****************************************************************************
#define	MAX_BULLET		(256)

#define	BULLET_WH		(5.0f)

//*****************************************************************************
// STRUCTíËã`
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;
	XMFLOAT3	pos;
	XMFLOAT3	rot;
	XMFLOAT3	scl;
	MATERIAL	material;
	float		spd;
	float		fWidth;
	float		fHeight;
	int			shadowIdx;
	BOOL		use;
} BULLET;

//*****************************************************************************
// Prototype declaration
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot);

BULLET* GetBullet(void);
