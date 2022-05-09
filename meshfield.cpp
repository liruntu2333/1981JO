//=============================================================================
//
// メッシュ地面の処理 [meshfield.cpp]
// Created by Li Runtu 2022 liruntu2333@gmail.com
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "meshfield.h"
#include "renderer.h"
#include "collision.h"
#include "camera.h"
#include "lightforshadow.h"
#include "rendertexture.h"
#include "depthshader.h"

//*****************************************************************************
// MACROS
//*****************************************************************************
#define TEXTURE_MAX		(1)

//*****************************************************************************
// GLOBALS
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11Buffer* g_IndexBuffer = NULL;	// インデックスバッファ

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };
static int							g_TexNo;

static XMFLOAT3		g_posField;								// ポリゴン表示位置の中心座標
static XMFLOAT3		g_rotField;								// ポリゴンの回転角

static int			g_nNumBlockXField, g_nNumBlockZField;	// ブロック数
static int			g_nNumVertexField;						// 総頂点数
static int			g_nNumVertexIndexField;					// 総インデックス数
static int			g_nNumPolygonField;						// 総ポリゴン数
static float		g_fBlockSizeXField, g_fBlockSizeZField;	// ブロックサイズ

static char* g_TextureName[] = {
	"data/TEXTURE/ground.jpg",
};

static VERTEX_3D* g_Vertex = NULL;

static XMFLOAT3		g_Center;
static float		g_Time = 0.0f;
static float		g_wave_frequency = 0.0f;
static float		g_wave_correction = 0.0f;
static float		g_wave_amplitude = 0.0f;

static BOOL			g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
	int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	g_posField = pos;

	g_rotField = rot;

	//  Initialize textrues
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	g_nNumBlockXField = nNumBlockX;
	g_nNumBlockZField = nNumBlockZ;

	g_nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	g_nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	g_nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	g_fBlockSizeXField = fBlockSizeX;
	g_fBlockSizeZField = fBlockSizeZ;

	g_Vertex = new VERTEX_3D[g_nNumVertexField];
	g_Center = XMFLOAT3(100.0f, 0.0f, 100.0f);
	g_Time = 0.0f;
	g_wave_frequency = 1.0f;
	g_wave_correction = 0.02f;
	g_wave_amplitude = 30.0f;

	for (int z = 0; z < (g_nNumBlockZField + 1); z++)
	{
		for (int x = 0; x < (g_nNumBlockXField + 1); x++)
		{
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x = -(g_nNumBlockXField / 2.0f) * g_fBlockSizeXField + x * g_fBlockSizeXField;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField - z * g_fBlockSizeZField;

			float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
			float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

			float len = (float)sqrt(dx * dx + dz * dz);

			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = cosf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;

			// In XY plane
			XMVECTOR nor = { sinf(dx * g_wave_correction),
				abs(cosf(len * g_wave_correction)),
				sinf(dz * g_wave_correction) };
			XMVector3Normalize(nor);
			XMStoreFloat3(&g_Vertex[z * (g_nNumBlockXField + 1) + x].Normal, nor);
			//g_Vertex[z * (g_nNumBlockXField + 1) + x].Normal = XMFLOAT3(0.0f, 1.0, 0.0f);

			g_Vertex[z * (g_nNumBlockXField + 1) + x].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			float texSizeX = 1.0f;
			float texSizeZ = 1.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.x = texSizeX * x;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.y = texSizeZ * z;
		}
	}

	// Initialize vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertexField;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// Initialize index buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * g_nNumVertexIndexField;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_IndexBuffer);

	{//頂点バッファの中身を埋める
		// 頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * g_nNumVertexField);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	{//インデックスバッファの中身を埋める
		// インデックスバッファのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short* pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for (int nCntVtxZ = 0; nCntVtxZ < g_nNumBlockZField; nCntVtxZ++)
		{
			if (nCntVtxZ > 0)
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1);
				nCntIdx++;
			}

			for (int nCntVtxX = 0; nCntVtxX < (g_nNumBlockXField + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
			}

			if (nCntVtxZ < (g_nNumBlockZField - 1))
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + g_nNumBlockXField;
				nCntIdx++;
			}
		}

		GetDeviceContext()->Unmap(g_IndexBuffer, 0);
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
	if (g_Load == FALSE) return;

	// インデックスバッファの解放
	if (g_IndexBuffer) {
		g_IndexBuffer->Release();
		g_IndexBuffer = NULL;
	}

	// 頂点バッファの解放
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{
	return;	// 処理をスキップ！

	float dt = 0.03f;

	for (int z = 0; z < g_nNumBlockZField; z++)
	{
		for (int x = 0; x < g_nNumBlockXField; x++)
		{
			float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
			float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

			// 波紋の中心点からの距離を得る
			float len = (float)sqrt(dx * dx + dz * dz);

			// 波の高さを、sin関数で得る
		//	g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = sinf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;
		}
	}
	g_Time += dt;

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	// 全頂点情報を毎回上書きしているのはDX11ではこの方が早いからです
	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * g_nNumVertexField);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshField(void)
{
	SetFuchi(0);
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Material 設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Ambient = { 2.0f, 2.0f, 2.0f, 1.0f };
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	material.Specular = { 0.0f, 0.0f, 0.0f, 1.0f };

	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// world matrixの初期化
	mtxWorld = XMMatrixIdentity();

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// world matrixの設定
	SetWorldMatrix(&mtxWorld);

	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_nNumVertexIndexField, 0, 0);

	//D3DXMATRIX lightViewMatrix, lightProjectionMatrix;
	//GetSLViewMatrix(lightViewMatrix);
	//GetSLProjectionMatrix(lightProjectionMatrix);

	//// Render the model using the shadow shader.
	// RenderShadow(GetDeviceContext(), g_nNumVertexIndexField, *reinterpret_cast<D3DXMATRIX*>(&mtxWorld), *reinterpret_cast<D3DXMATRIX*>(&GetCamera()->mtxView),
	//	 *reinterpret_cast<D3DXMATRIX*>(&GetCamera()->mtxProjection),
	//	 lightViewMatrix,
	//	lightProjectionMatrix, g_Texture[0], GetRTShaderResourceView(), GetSLPosition(),
	//	GetSLAmbientColor(), GetSLDiffuseColor());
}

bool RenderFieldWithDepthShader(D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix)
{
	// Get the Identity world matrix.
	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// world matrixの初期化
	mtxWorld = XMMatrixIdentity();

	mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VERTEX_3D);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (!RenderDepthShader(GetDeviceContext(),
		g_nNumVertexIndexField,
		0,
		xmmatrix2d3dmatrix(mtxWorld),
		lightViewMatrix,
		lightProjectionMatrix)) return false;

	return true;
}

BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3* HitPosition, XMFLOAT3* Normal)
{
	XMFLOAT3 start = pos;
	XMFLOAT3 end = pos;
	XMFLOAT3 org = *HitPosition;

	// 少し上から、ズドーンと下へレイを飛ばす
	start.y += 1000.0f;
	end.y -= 1000.0f;

	// 処理を高速化する為に全検索ではなくて、座標からポリゴンを割り出すと
	float fz = (g_nNumBlockXField / 2.0f) * g_fBlockSizeXField;
	float fx = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField;
	int sz = (int)((-start.z + fz) / g_fBlockSizeZField);
	int sx = (int)((start.x + fx) / g_fBlockSizeXField);
	int ez = sz + 1;
	int ex = sx + 1;

	if ((sz < 0) || (sz > g_nNumBlockZField - 1) ||
		(sx < 0) || (sx > g_nNumBlockXField - 1))
	{
		*Normal = { 0.0f, 1.0f, 0.0f };
		return FALSE;
	}

	// 必要数分検索を繰り返す
	for (int z = sz; z < ez; z++)
	{
		for (int x = sx; x < ex; x++)
		{
			XMFLOAT3 p0 = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p1 = g_Vertex[z * (g_nNumBlockXField + 1) + (x + 1)].Position;
			XMFLOAT3 p2 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p3 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + (x + 1)].Position;

			// 三角ポリゴンだから２枚分の当たり判定
			BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}

			ans = RayCast(p1, p2, p3, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}
		}
	}

	// 何処にも当たっていなかった時
	*HitPosition = org;
	return FALSE;
}