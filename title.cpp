#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"

#define TEXTURE_WIDTH				(SCREEN_WIDTH)
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)
#define TEXTURE_MAX					(3)

#define TEXTURE_WIDTH_LOGO			(480)
#define TEXTURE_HEIGHT_LOGO			(80)			

static ID3D11Buffer* g_VertexBuffer = NULL;
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/galaxy.png",
	"data/TEXTURE/title_logo.png",
	"data/TEXTURE/effect000.jpg",
};

static BOOL						g_Use;
static float					g_w, g_h;					   
static float					g_logoW, g_logoH;			   
static XMFLOAT3					g_Pos;
static int						g_TexNo;

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

HRESULT InitTitle(void)
{
	ID3D11Device* pDevice = GetDevice();

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_logoW = TEXTURE_WIDTH_LOGO;
	g_logoH = TEXTURE_HEIGHT_LOGO;
	g_Pos = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	g_TexNo = 0;

	alpha = 1.0f;
	flag_alpha = TRUE;

	PlaySound(SOUND_LABEL_BGM_sample000);

	g_Load = TRUE;
	return S_OK;
}

void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

void UpdateTitle(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{ 
		SetFade(FADE_OUT, MODE_GAME);
	}
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_GAME);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_GAME);
	}

	g_w += TEXTURE_WIDTH * 0.0001f;
	g_h += TEXTURE_HEIGHT * 0.0001f;

	g_logoW += TEXTURE_WIDTH_LOGO * 0.0005f;
	g_logoH += TEXTURE_HEIGHT_LOGO * 0.0005f;

#ifdef _DEBUG	 
#endif
}

void DrawTitle(void)
{
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	SetWorldViewProjection2D();

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_logoW, g_logoH, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		GetDeviceContext()->Draw(4, 0);
	}

}