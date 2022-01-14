////////////////////////////////////////////////////////////////////////////////
// Filename: shader.hlsl
////////////////////////////////////////////////////////////////////////////////

//////////////////////
// CONSTANT BUFFERS //
//////////////////////

cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}

cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}

cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}

// Material coefficients
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte境界用
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ライト用バッファ
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte境界用
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
	float		Dummy[3];//16byte境界用
};

// フォグ用バッファ
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// 縁取り用バッファ
cbuffer Fuchi : register(b6)
{
	int			fuchi;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

cbuffer LIGHT2 : register (b8) // TODO : Set buffers in VS, PS ---- DONE
{
	float3 lightPosition;
	float padding;
}

cbuffer LightViewBuffer : register(b9) // TODO : Set buffers in VS, PS ---- DONE
{
	matrix LightView;
}

cbuffer LightProjectionBuffer : register(b10) // TODO : Set buffers in VS, PS ---- DONE
{
	matrix LightProjection;
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
void VertexShaderPolygon(	in  float4 inPosition		: POSITION0,
							in  float4 inNormal		: NORMAL0,
							in  float4 inDiffuse		: COLOR0,
							in  float2 inTexCoord		: TEXCOORD0,

							out float4 outPosition	: SV_POSITION,
							out float4 outNormal		: NORMAL0,
							out float2 outTexCoord	: TEXCOORD0,
							out float4 outDiffuse		: COLOR0,
							out float4 outWorldPos    : POSITION0,
							out float4 outLightViewPosition : TEXCOORD1,	// TODO : add parameters to PSlayout ---- NO NEED
							out float3 outLightPos		:	TEXCOORD2)		// TODO : add parameters to PSlayout ---- NO NEED
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);

	outDiffuse = inDiffuse;

	// Calculate the position of the vertice as viewed by the light source.
	outLightViewPosition = mul(float4(inPosition.xyz, 1.f), World);
	outLightViewPosition = mul(outLightViewPosition, LightView);
	outLightViewPosition = mul(outLightViewPosition, LightProjection);

	// Calculate the position of the vertex in the world.
	float4 worldPosition = mul(inPosition, World);

	// Determine the light position based on the position of the light and the position of the vertex in the world.
	outLightPos = lightPosition.xyz - worldPosition.xyz;

	// Normalize the light position vector.
	outLightPos = normalize(outLightPos);
}



//////////////
// TEXTURES //
//////////////
Texture2D		g_Texture : register( t0 );
Texture2D		g_depthMapTexture : register(t1);		// TODO : add depthTexture to render resource ---- DONE

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState	g_SamplerState		: register( s0 );
SamplerState	g_SamplerStateClamp : register( s1 );	// TODO: add sampler state clamp for shadow map in render ---- DONE


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
void PixelShaderPolygon(	in  float4 inPosition		: SV_POSITION,
							in  float4 inNormal		: NORMAL0,
							in  float2 inTexCoord		: TEXCOORD0,
							in  float4 inDiffuse		: COLOR0,
							in  float4 inWorldPos      : POSITION0,
							in	float4 inLightViewPosition : TEXCOORD1,		// TODO : add parameters to PSlayout ---- NO NEED
							in	float3 inLightPos		:	TEXCOORD2,		// TODO : add parameters to PSlayout ---- NO NEED

							out float4 outDiffuse		: SV_Target )
{
	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float4 ambient;
	float4 diffuse;
	float4 specular;

	if (Material.noTexSampling == 0)
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else
	{
		color = inDiffuse;
	}

	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		if (fuchi == 1) // Object is casting shadow upon others, shouldn't use depth texture.
		{
			float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
			float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

			for (int i = 0; i < 5; i++)
			{
				float3 lightDir;
				float light;

				ambient = Light.Ambient[0] * Material.Ambient;

				if (Light.Flags[i].y == 1)
				{
					if (Light.Flags[i].x == 1)
					{
						lightDir = normalize(-Light.Direction[i].xyz);
						float3 view_dir = normalize(Camera.xyz - inWorldPos.xyz);
						float3 half_vec = normalize(lightDir + view_dir);
						light = dot(lightDir, normalize(inNormal.xyz));

						diffuse = light * Material.Diffuse * Light.Diffuse[i] * color * 3.0f;

						float4 specular = Material.Specular * pow(max(.0f, dot(normalize(inNormal.xyz), half_vec)), 100.f);
						//light = 0.5 - 0.5 * light;
						tempColor = ambient + diffuse + specular;
					}
					else if (Light.Flags[i].x == 2)
					{
						lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
						light = dot(lightDir, inNormal.xyz);

						tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

						float distance = length(inWorldPos - Light.Position[i]);

						float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
						tempColor *= att;
					}
					else
					{
						tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
					}

					outColor += tempColor;
				}
			}

			color = outColor;
			color.a = inDiffuse.a * Material.Diffuse.a;
		}
		else
		{
			// Set the bias value for fixing the floating point precision issues.
			bias = 0.0001f;

			// Calculate the projected texture coordinates.
			projectTexCoord.x = inLightViewPosition.x / inLightViewPosition.w / 2.0f + 0.5f;
			projectTexCoord.y = -inLightViewPosition.y / inLightViewPosition.w / 2.0f + 0.5f;

			// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
			if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
			{

				// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
				depthValue = g_depthMapTexture.Sample(g_SamplerStateClamp, projectTexCoord).r;

				// Calculate the depth of the light.
				lightDepthValue = inLightViewPosition.z / inLightViewPosition.w;

				// Subtract the bias from the lightDepthValue.
				lightDepthValue = lightDepthValue - bias;

				// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
				// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
				if (lightDepthValue < depthValue)
				{
					float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
					float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

					for (int i = 0; i < 5; i++)
					{
						float3 lightDir;
						float light;

						if (Light.Flags[i].y == 1)
						{
							if (Light.Flags[i].x == 1)
							{

								float3 light_dir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
								float3 view_dir = normalize(Camera.xyz - inWorldPos.xyz);
								float3 half_vec = normalize(light_dir + view_dir);
								float distance = length(inWorldPos.xyz - Light.Position[i].xyz);
								float4 specular = Material.Specular / pow(distance, 2.f) * pow(max(.0f, dot(inNormal.xyz, half_vec)), 100.f) * 255.f;

								lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
								light = dot(lightDir, inNormal.xyz);

								tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

								lightDir = normalize(Light.Direction[i].xyz);
								light = dot(lightDir, inNormal.xyz);

								light = 0.5 - 0.5 * light;
								tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
								tempColor.xyz = tempColor.xyz + specular.xyz;
							}
							else if (Light.Flags[i].x == 2)
							{
								lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
								light = dot(lightDir, inNormal.xyz);

								tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

								float distance = length(inWorldPos - Light.Position[i]);

								float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
								tempColor *= att;
							}
							else
							{
								tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
							}

							outColor += tempColor;
						}
					}

					color = outColor;
					color.a = inDiffuse.a * Material.Diffuse.a;
				}
			}
			else // Current pixel out of view of light, set the traditional lighting.
			{
			}
		}

	}

	////tempColor.xyz = normalize(inNormal.xyz);
	//// Blin-Phong Shading
	//float3 ambient = { .0f, .0f, .0f };
	//float3 diffuse = { .0f, .0f, .0f };
	//float3 specular = { .0f, .0f, .0f };
	//float3 light_dir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
	//float3 view_dir = normalize(Camera.xyz - inWorldPos.xyz);
	//float3 half_vec = normalize(light_dir + view_dir);
	//float distance = length(inWorldPos.xyz - Light.Position[i].xyz);
	//tempColor = color * Material.Diffuse * 0.1f;
	//ambient = Light.Ambient[i].xyz * Material.Ambient.xyz / 100.f;
	//diffuse = color * Light.Diffuse[i].xyz / pow(distance, 2.f) * max(.0f, dot(inNormal.xyz, light_dir)) * 255.f;
	//specular = Material.Specular.xyz / pow(distance, 2.f) * pow(max(.0f, dot(inNormal.xyz, half_vec)), 100.f) * 255.f;
	//tempColor.xyz = ambient + diffuse + specular;

	//lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
	//light = dot(lightDir, inNormal.xyz);

	//tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
	

	//フォグ
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
		clip(outDiffuse.a - .1f);
	}
	else
	{
		outDiffuse = color;
		clip(outDiffuse.a - .1f);
	}
	outDiffuse = color;
	clip(outDiffuse.a - .1f);

}
