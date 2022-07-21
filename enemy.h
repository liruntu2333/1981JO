#pragma once

#define MAX_ENEMY		(5)

#define	ENEMY_SIZE		(5.0f)

struct ENEMY
{
	XMFLOAT4X4			mtxWorld;			  
	XMFLOAT3			pos;				 
	XMFLOAT3			rot;				 
	XMFLOAT3			scl;				 

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				 
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	 

	float				spd;				 
	float				size;				 
	int					shadowIdx;			 

	INTERPOLATION_DATA* tbl_adr;			 
	int					tbl_size;			 
	float				move_time;			 

	XMFLOAT4			quaternion;	 
	XMFLOAT3			upVector;	 
};

HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetEnemy(void);

bool RenderEnemyWithDepthShader(D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix);