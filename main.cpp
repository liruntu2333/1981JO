//=============================================================================
//
// ���C������ [main.cpp]
// Created by Li Runtu 2022 liruntu2333@gmail.com
//
//=============================================================================
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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"Engine / Camera :ZC QE YN UM R / Player:�� �� �� ���@Space"		// �E�C���h�E�̃L���v�V������

/////////////
// GLOBALS //
/////////////
static const float SCREEN_DEPTH = 10000.0f;
static const float SCREEN_NEAR = 1.0f;
static const int SHADOWMAP_WIDTH = 10000;
static const int SHADOWMAP_HEIGHT = 10000;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
bool RenderSceneToTexture();

//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int	g_Mode = MODE_TITLE;					// �N�����̉�ʂ�ݒ�

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
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
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// �E�B���h�E�̍����W
		CW_USEDEFAULT,																		// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// ����������(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
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

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	// Initialize Depth Shader
	InitDepthShader(GetDevice(), hWnd);

	// Initialize Render texture helper
	InitRenderTex(GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);

	InitLight();

	// Initialize the shadow light object.
	{
		SetSLPosition(0.f, 800.0f, 0.f);
		SetSLLookAt(0.0f, 0.0f, 0.0f);
		GenerateSLViewMatrix();
		GenerateSLProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

		// Set Light position
		D3DXVECTOR3 lightPosition = GetSLPosition();
		LIGHT2 l2{ {lightPosition.x, lightPosition.y, lightPosition.z}, 0.f };
		SetShadowLight(&l2);

		// Set Light View & Proj Matrix
		D3DXMATRIX matrix;
		GetSLViewMatrix(matrix);
		SetLightViewMatrix(&d3dmatrix2xmmatrix(matrix));
		GetSLProjectionMatrix(matrix);
		SetLightProjMatrix(&d3dmatrix2xmmatrix(matrix));
	}

	InitCamera();

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	// �T�E���h�̏�����
	InitSound(hWnd);


	// ���C�g��L����
	SetLightEnable(TRUE);

	// �w�ʃ|���S�����J�����O
	SetRasterizeState(CULL_MODE_BACK);


	// �t�F�[�h�̏�����
	InitFade();

	// �ŏ��̃��[�h���Z�b�g
	SetMode(g_Mode);	// ������SetMode�̂܂܂ŁI

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	// �I���̃��[�h���Z�b�g
	SetMode(MODE_MAX);


	// �T�E���h�I������
	UninitSound();

	// �J�����̏I������
	UninitCamera();

	//���͂̏I������
	UninitInput();

	// �����_���[�̏I������
	UninitRenderer();

	// Shut down Depth Shader
	ShutdownDS();
	// Shut down Render to texture helper
	ShutdownRenderTex();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͂̍X�V����
	UpdateInput();

	// ���C�g�̍X�V����
	UpdateLight();

	// �J�����X�V
	UpdateCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̍X�V
		UpdateTitle();
		break;

	case MODE_GAME:			// �Q�[����ʂ̍X�V
		UpdateGame();
		break;

	case MODE_RESULT:		// ���U���g��ʂ̍X�V
		UpdateResult();
		break;
	}

	// �t�F�[�h�����̍X�V
	UpdateFade();


}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	SetCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̕`��
		SetViewPort(TYPE_FULL_SCREEN);

		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawTitle();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
		break;

	case MODE_GAME:			// �Q�[����ʂ̕`��
		// Implement RenderSceneToTexture() from tutorial40, GraphicsClass::RenderSceneToTexture()
		// before rendering the gameobj, render enemy & player to the texture for shadow shader
		if(!RenderSceneToTexture()) 
		{
#ifdef _DEBUG
			PrintDebugProc("render depth texture failed\n");
#endif // _DEBUG
		}

		DrawGame();
		break;

	case MODE_RESULT:		// ���U���g��ʂ̕`��
		SetViewPort(TYPE_FULL_SCREEN);

		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawResult();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
		break;
	}

	// �t�F�[�h�`��
	DrawFade();

#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
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



//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	// ���[�h��ς���O�ɑS������������������Ⴄ

	// �^�C�g����ʂ̏I������
	UninitTitle();

	// �Q�[����ʂ̏I������
	UninitGame();

	// ���U���g��ʂ̏I������
	UninitResult();


	g_Mode = mode;	// ���̃��[�h���Z�b�g���Ă���

	switch (g_Mode)
	{
	case MODE_TITLE:
		// �^�C�g����ʂ̏�����
		InitTitle();
		break;

	case MODE_GAME:
		// �Q�[����ʂ̏�����
		InitGame();
		break;

	case MODE_RESULT:
		// ���U���g��ʂ̏�����
		InitResult();
		break;

		// �Q�[���I�����̏���
	case MODE_MAX:
		// �G�l�~�[�̏I������
		UninitEnemy();

		// �v���C���[�̏I������
		UninitPlayer();
		break;
	}
}

//=============================================================================
// ���[�h�̎擾
//=============================================================================
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

	// Set the render target to be the render to texture.
	SetRTRenderTarget(GetDeviceContext());

	// Clear the render to texture.
	ClearRTRenderTarget(GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the light view matrix based on the light's position.
	GenerateSLViewMatrix();

	// Get the view and orthographic matrices from the light object.
	GetSLViewMatrix(lightViewMatrix);
	GetSLProjectionMatrix(lightProjectionMatrix);

	// Render Player model with depth shader.
	result = RenderPlayerWithDepthShader(lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// Render Enemy model with depth shader.
	result = RenderEnemyWithDepthShader(lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// Render the ground model with the depth shader.
	//result = RenderFieldWithDepthShader(lightViewMatrix, lightProjectionMatrix);
	//if (!result)
	//{
	//	return false;
	//}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	// Reset the viewport back to the original.
	ResetRenderer();

	// Setup depth texture in formal render.
	ID3D11ShaderResourceView* pShaderResourceView = GetRTShaderResourceView();
	GetDeviceContext()->PSSetShaderResources(1, 1, &pShaderResourceView);

	return true;
}