#pragma once

#define MAX_PLAYER		(1)					 

#define	PLAYER_SIZE		(5.0f)				 

struct PLAYER
{
	XMFLOAT3			pos;		 
	XMFLOAT3			rot;		 
	XMFLOAT3			scl;		 

	XMFLOAT4X4			mtxWorld;	  

	BOOL				load;
	DX11_MODEL			model;		 

	float				spd;		 
	float				dir;		 
	float				size;		 
	int					shadowIdx;	 
	BOOL				use;

	INTERPOLATION_DATA* tbl_adr;	 
	int					tbl_size;	 
	float				move_time;	 

	PLAYER* parent;	 

	XMFLOAT4			quaternion;	 
	XMFLOAT3			upVector;	 
};

HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);

bool RenderPlayerWithDepthShader(D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix);