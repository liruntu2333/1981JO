#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "tree.h"

#define TEXTURE_MAX			(5)

#define	TREE_WIDTH			(50.0f)			 
#define	TREE_HEIGHT			(80.0f)			 

#define	MAX_TREE			(256)			 

typedef struct
{
	XMFLOAT3	pos;			 
	XMFLOAT3	scl;			 
	MATERIAL	material;		 
	float		fWidth;			 
	float		fHeight;		 
	int			nIdxShadow;		 
	BOOL		bUse;			 
} TREE;

HRESULT MakeVertexTree(void);

static ID3D11Buffer* g_VertexBuffer = NULL;	 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };

static TREE					g_aTree[MAX_TREE];	 
static int					g_TexNo;
static BOOL					g_bAlpaTest;		 
static BOOL					g_Load = FALSE;

static char* g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/monolith.png",
	"data/TEXTURE/tree002.png",
	"data/TEXTURE/tree003.png",
	"data/TEXTURE/tree004.png",
	"data/TEXTURE/tree005.png",
};

HRESULT InitTree(void)
{
	MakeVertexTree();

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[0],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	for (int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		ZeroMemory(&g_aTree[nCntTree].material, sizeof(g_aTree[nCntTree].material));
		g_aTree[nCntTree].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aTree[nCntTree].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aTree[nCntTree].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aTree[nCntTree].fWidth = TREE_WIDTH;
		g_aTree[nCntTree].fHeight = TREE_HEIGHT;
		g_aTree[nCntTree].bUse = FALSE;
	}

	g_bAlpaTest = TRUE;
	SetTree(XMFLOAT3(0.0f, 0.0f, 0.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetTree(XMFLOAT3(200.0f, 0.0f, 0.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetTree(XMFLOAT3(-200.0f, 0.0f, 0.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetTree(XMFLOAT3(0.0f, 0.0f, 200.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetTree(XMFLOAT3(0.0f, 0.0f, -200.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	g_Load = TRUE;
	return S_OK;
}

void UninitTree(void)
{
	if (g_Load == FALSE) return;

	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{ 
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{ 
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

void UpdateTree(void)
{
	for (int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		if (g_aTree[nCntTree].bUse)
		{
		}
	}

#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE : TRUE;
	}

#endif
}

void DrawTree(void)
{
	if (g_bAlpaTest == TRUE)
	{
		SetAlphaTestEnable(TRUE);
	}

	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_TREE; i++)
	{
		if (g_aTree[i].bUse)
		{
			mtxWorld = XMMatrixIdentity();

			mtxView = XMLoadFloat4x4(&cam->mtxView);

			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			mtxScl = XMMatrixScaling(g_aTree[i].scl.x, g_aTree[i].scl.y, g_aTree[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			mtxTranslate = XMMatrixTranslation(g_aTree[i].pos.x, g_aTree[i].pos.y, g_aTree[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			SetWorldMatrix(&mtxWorld);

			SetMaterial(g_aTree[i].material);

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i % TEXTURE_MAX]);

			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetLightEnable(TRUE);

	SetAlphaTestEnable(FALSE);
}

HRESULT MakeVertexTree(void)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

int SetTree(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxTree = -1;

	for (int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		if (!g_aTree[nCntTree].bUse)
		{
			g_aTree[nCntTree].pos = pos;
			g_aTree[nCntTree].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_aTree[nCntTree].fWidth = fWidth;
			g_aTree[nCntTree].fHeight = fHeight;
			g_aTree[nCntTree].bUse = TRUE;

			nIdxTree = nCntTree;

			break;
		}
	}

	return nIdxTree;
}