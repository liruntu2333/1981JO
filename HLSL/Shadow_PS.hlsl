Texture2D g_DiffuseMap : register(t0);
SamplerState g_Sam : register(s0);

struct VertexPosHTex
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
};

void PS(VertexPosHTex pIn)
{
    float4 diffuse = g_DiffuseMap.Sample(g_Sam, pIn.Tex);

    // clip transparent pixel
    clip(diffuse.a - 0.1f);
}
