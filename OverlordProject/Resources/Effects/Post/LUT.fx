//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Mirror;
	AddressV = Mirror;
};

float4x4 gMatrixView : VIEW;
float4x4 gMatrixViewInverse : VIEWINVERSE;

Texture2D gTexture;
Texture2D gLut;

float colourCount = 32.f;
float lutW = 1024.f;
float lutH = 32.f;

DepthStencilState DefaultDepthStencilState
{
	DepthEnable = TRUE;
};

RasterizerState BackCulling
{
	CullMode = BACK;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Position = float4(input.Position, 1.f);
	output.TexCoord = input.TexCoord;

	return output;
}

//PIXEL SHADER
//------------
float4 PS(PS_INPUT input) : SV_Target
{
	// Based on: https://halisavakis.com/my-take-on-shaders-color-grading-with-look-up-textures-lut/

	float maxColour = colourCount - 1.0;
	float4 colour = gTexture.Sample(samPoint, input.TexCoord);
	float halfX = .5f / lutW;
	float halfY = .5f / lutH;
	float threshold = maxColour / colourCount;

	float xOffset = halfX + colour.r * threshold / colourCount;
	float yOffset = halfY + colour.g * threshold;
	float cell = floor(colour.b * maxColour);

	float2 lutPos = float2(cell / colourCount + xOffset, 1 - yOffset);
	float4 gradedCol = gLut.Sample(samPoint, lutPos);

	return float4(
		lerp(
			colour.xyz,
			gradedCol.xyz,
			1.f),
		1.f);
}


//TECHNIQUE
//---------
technique11 LUT
{
	pass P0
	{
		// Set states...
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
		SetRasterizerState(BackCulling);
		SetDepthStencilState(DefaultDepthStencilState, 0);
	}
}

