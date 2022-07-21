#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"

#define TEXTURE_WIDTH				(SCREEN_WIDTH)
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	
#define TEXTURE_MAX					(1)

#define	FADE_RATE					(0.02f)			 

static ID3D11Buffer* g_VertexBuffer = NULL;
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/fade_black.png",
};

static BOOL						g_Use;
static float					g_w, g_h;					   
static XMFLOAT3					g_Pos;
static int						g_TexNo;

FADE							g_Fade = FADE_IN;			 
int								g_ModeNext;					 
XMFLOAT4						g_Color;					 

static BOOL						g_Load = FALSE;

HRESULT InitFade(void)
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
	g_Pos = { 0.0f, 0.0f, 0.0f };
	g_TexNo = 0;

	g_Fade = FADE_IN;
	g_Color = { 1.0, 0.0, 0.0, 1.0 };

	g_Load = TRUE;
	return S_OK;
}

void UninitFade(void)
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

void UpdateFade(void)
{
	if (g_Fade != FADE_NONE)
	{ 
		if (g_Fade == FADE_OUT)
		{ 
			g_Color.w += FADE_RATE;		 
			if (g_Color.w >= 1.0f)
			{
				StopSound();

				g_Color.w = 1.0f;
				SetFade(FADE_IN, g_ModeNext);

				SetMode(g_ModeNext);
			}
		}
		else if (g_Fade == FADE_IN)
		{ 
			g_Color.w -= FADE_RATE;		 
			if (g_Color.w <= 0.0f)
			{
				g_Color.w = 0.0f;
				SetFade(FADE_NONE, g_ModeNext);
			}
		}
	}

#ifdef _DEBUG	 
#endif
}

void DrawFade(void)
{
	if (g_Fade == FADE_NONE) return;	 

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH / 2, TEXTURE_WIDTH / 2, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f,
			g_Color);

		GetDeviceContext()->Draw(4, 0);
	}
}

void SetFade(FADE fade, int modeNext)
{
	g_Fade = fade;
	g_ModeNext = modeNext;
}

FADE GetFade(void)
{
	return g_Fade;
}