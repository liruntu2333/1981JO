#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "light.h"
#include "meshfield.h"
#include "meshwall.h"
#include "tree.h"
#include "collision.h"
#include "bullet.h"
#include "score.h"
#include "sound.h"
#include "particle.h"

#include "title.h"
#include "game.h"
#include "result.h"
#include "fade.h"

#include "lightforshadow.h"
#include "depthshader.h"
#include "rendertexture.h"

#define CLASS_NAME		"AppClass"			 
#define WINDOW_NAME		"1981JO Engine"		 

static const float SCREEN_DEPTH = 10000.0f;
static const float SCREEN_NEAR = 1.0f;
static const int SHADOWMAP_WIDTH = 10000;
static const int SHADOWMAP_HEIGHT = 10000;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
bool RenderSceneToTexture();

long g_MouseX = 0;
long g_MouseY = 0;

#ifdef _DEBUG
int		g_CountFPS;							 
char	g_DebugStr[2048] = WINDOW_NAME;		 

#endif

int	g_Mode = MODE_TITLE;					 

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	 
	UNREFERENCED_PARAMETER(lpCmdLine);		 

	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;

	RegisterClassEx(&wcex);

	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		 
		CW_USEDEFAULT,																		 
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									 
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	 
		NULL,
		NULL,
		hInstance,
		NULL);

	if (FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	timeBeginPeriod(1);	 
	dwExecLastTime = dwFPSLastTime = timeGetTime();	 
	dwCurrentTime = dwFrameCount = 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{ 
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	 
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				 
				dwFrameCount = 0;							 
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	 
			{
				dwExecLastTime = dwCurrentTime;	 

#ifdef _DEBUG	 
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			 
				Draw();				 

#ifdef _DEBUG	 
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				 

	UnregisterClass(CLASS_NAME, wcex.hInstance);

	Uninit();

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitDepthShader(GetDevice(), hWnd);

	InitRenderTex(GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);

	InitLight();

	{
		SetSLPosition(0.f, 800.0f, 0.f);
		SetSLLookAt(0.0f, 0.0f, 0.0f);
		GenerateSLViewMatrix();
		GenerateSLProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

		D3DXVECTOR3 lightPosition = GetSLPosition();
		LIGHT2 l2{ {lightPosition.x, lightPosition.y, lightPosition.z}, 0.f };
		SetShadowLight(&l2);

		D3DXMATRIX matrix;
		GetSLViewMatrix(matrix);
		SetLightViewMatrix(&d3dmatrix2xmmatrix(matrix));
		GetSLProjectionMatrix(matrix);
		SetLightProjMatrix(&d3dmatrix2xmmatrix(matrix));
	}

	InitCamera();

	InitInput(hInstance, hWnd);

	InitSound(hWnd);

	SetLightEnable(TRUE);

	SetRasterizeState(CULL_MODE_BACK);

	InitFade();

	SetMode(g_Mode);	 

	return S_OK;
}

void Uninit(void)
{
	SetMode(MODE_MAX);

	UninitSound();

	UninitCamera();

	UninitInput();

	UninitRenderer();

	ShutdownDS();
	ShutdownRenderTex();
}

void Update(void)
{
	UpdateInput();

	UpdateLight();

	UpdateCamera();

	switch (g_Mode)
	{
	case MODE_TITLE:		 
		UpdateTitle();
		break;

	case MODE_GAME:			 
		UpdateGame();
		break;

	case MODE_RESULT:		 
		UpdateResult();
		break;
	}

	UpdateFade();
}

void Draw(void)
{
	Clear();

	SetCamera();

	switch (g_Mode)
	{
	case MODE_TITLE:		 
		SetViewPort(TYPE_FULL_SCREEN);

		SetDepthEnable(FALSE);

		SetLightEnable(FALSE);

		DrawTitle();

		SetLightEnable(TRUE);

		SetDepthEnable(TRUE);
		break;

	case MODE_GAME:
		if (!RenderSceneToTexture())
		{
#ifdef _DEBUG
			PrintDebugProc("render depth texture failed\n");
#endif  
		}

		DrawGame();
		break;

	case MODE_RESULT:		 
		SetViewPort(TYPE_FULL_SCREEN);

		SetDepthEnable(FALSE);

		SetLightEnable(FALSE);

		DrawResult();

		SetLightEnable(TRUE);

		SetDepthEnable(TRUE);
		break;
	}

	DrawFade();

#ifdef _DEBUG
	DrawDebugProc();
#endif

	Present();
}

long GetMousePosX(void)
{
	return g_MouseX;
}

long GetMousePosY(void)
{
	return g_MouseY;
}

#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif

void SetMode(int mode)
{
	UninitTitle();

	UninitGame();

	UninitResult();

	g_Mode = mode;	 

	switch (g_Mode)
	{
	case MODE_TITLE:
		InitTitle();
		break;

	case MODE_GAME:
		InitGame();
		break;

	case MODE_RESULT:
		InitResult();
		break;

	case MODE_MAX:
		UninitEnemy();

		UninitPlayer();
		break;
	}
}

int GetMode(void)
{
	return g_Mode;
}

bool RenderSceneToTexture()
{
	ID3D11ShaderResourceView* null = nullptr;
	GetDeviceContext()->PSSetShaderResources(1, 1, &null);

	D3DXMATRIX lightViewMatrix, lightProjectionMatrix;

	bool result;

	SetRTRenderTarget(GetDeviceContext());

	ClearRTRenderTarget(GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	GenerateSLViewMatrix();

	GetSLViewMatrix(lightViewMatrix);
	GetSLProjectionMatrix(lightProjectionMatrix);

	result = RenderPlayerWithDepthShader(lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderEnemyWithDepthShader(lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	ResetRenderer();

	ID3D11ShaderResourceView* pShaderResourceView = GetRTShaderResourceView();
	GetDeviceContext()->PSSetShaderResources(1, 1, &pShaderResourceView);

	return true;
}