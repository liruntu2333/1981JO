#include "main.h"
#include "renderer.h"
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "score.h"

#define TEXTURE_WIDTH				(SCREEN_WIDTH)
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)
#define TEXTURE_MAX					(3)

#define TEXTURE_WIDTH_LOGO			(480)
#define TEXTURE_HEIGHT_LOGO			(80)

static ID3D11Buffer* g_VertexBuffer = NULL;
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/earth_at_moon.jpg",
	"data/TEXTURE/result_logo.png",
	"data/TEXTURE/number.png",
};

static BOOL						g_Use;
static float					g_w, g_h;					   
static XMFLOAT3					g_Pos;
static int						g_TexNo;

static BOOL						g_Load = FALSE;

HRESULT InitResult(void)
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
	g_Pos = { g_w / 2, 50.0f, 0.0f };
	g_TexNo = 0;

	PlaySound(SOUND_LABEL_BGM_sample002);

	g_Load = TRUE;
	return S_OK;
}

void UninitResult(void)
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

void UpdateResult(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{ 
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}

#ifdef _DEBUG	 

#endif
}

void DrawResult(void)
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

		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		int number = GetScore();
		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			float x = (float)(number % 10);

			float pw = 16 * 4;			 
			float ph = 32 * 4;			 
			float px = 600.0f - i * pw;	 
			float py = 470.0f;			 

			float tw = 1.0f / 10;		 
			float th = 1.0f / 1;		 
			float tx = x * tw;			 
			float ty = 0.0f;			 

			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			GetDeviceContext()->Draw(4, 0);

			number /= 10;
		}
	}
}