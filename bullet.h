#pragma once

#define	MAX_BULLET		(256)

#define	BULLET_WH		(5.0f)

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

HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot);

BULLET* GetBullet(void);
