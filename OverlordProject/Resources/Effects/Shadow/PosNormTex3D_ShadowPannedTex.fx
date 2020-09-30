float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.01f;

float4x4 gMatrixView : VIEW;
float4x4 gMatrixViewInverse : VIEWINVERSE;

float gTotalElapsed;

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;// or Mirror or Clamp or Border
	AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerComparisonState cmpSampler
{
   // sampler state
   Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
   AddressU = MIRROR;
   AddressV = MIRROR;
 
   // sampler comparison state
   ComparisonFunc = LESS_EQUAL;
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 lPos : TEXCOORD1;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	
	output.pos = mul(float4(input.pos, 1.f), gWorldViewProj);
	output.normal = mul(input.normal, (float3x3)gWorld);
	output.texCoord = input.texCoord;

	//TODO: complete Vertex Shader
	//Hint: Don't forget to project our position to light clip space and store it in lPos
	
	output.lPos = mul(float4(input.pos, 1.f), gWorldViewProj_Light);

	return output;
}

float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
	return float2( u * 1.0f / 1280, v * 1.0f / 720 );
}

float EvaluateShadowMap(float4 lpos)
{
	// Rehomogenization
	lpos.xyz /= lpos.w;

 	if(lpos.x < -1.0f || lpos.x > 1.0f || lpos.y < -1.0f || lpos.y > 1.0f || lpos.z < 0.0f  || lpos.z > 1.0f) 
 		return 1.f;

	// clip space to tex space
    lpos.x = lpos.x / 2.f + 0.5f;
    lpos.y = lpos.y / -2.f + 0.5f;

    // Shadow map bias
    lpos.z -= gShadowMapBias;

    // PCF sampling for shadow map
    float sum = 0;
    float x, y;

    // PCF filtering on a 4 x 4 texel neighborhood
    for (y = -1.5; y <= 1.5; y += 1.0)
    {
        for (x = -1.5; x <= 1.5; x += 1.0)
            sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x,y), lpos.z);
    }

    float shadowMapDepth = sum / 16.f;

	if (shadowMapDepth <= lpos.z)
		return 0.2f;

	return shadowMapDepth;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float shadowValue = EvaluateShadowMap(input.lPos);

	float4 diffuseColor = gDiffuseMap.Sample(samLinear, input.texCoord + float2(sin(gTotalElapsed / 10.f), 0.f));
	float4 diffuseColor2 = gDiffuseMap.Sample(samLinear, input.texCoord + float2(0.f, sin(gTotalElapsed / 10.f)));

	float3 color_rgb = diffuseColor.rgb;
	float color_a = diffuseColor.a;

	// HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	return float4( saturate(lerp(diffuseColor.rgb, diffuseColor2.rgb, abs(cos(gTotalElapsed)))) * shadowValue * diffuseStrength, color_a );
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

