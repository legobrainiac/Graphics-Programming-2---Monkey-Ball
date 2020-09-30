//************
// VARIABLES *
//************
float4x4 gMatrixView : VIEW;
float4x4 gMatrixViewInverse : VIEWINVERSE;

cbuffer cbPerObject
{
    float4x4 m_MatrixWorldViewProj : WORLDVIEWPROJECTION;
    float4x4 m_MatrixWorld : WORLD;
    float3 m_LightDir = { 0.2f, -1.0f, 0.2f };
}

RasterizerState FrontCulling
{
    CullMode = NONE;
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap; // of Mirror of Clamp of Border
    AddressV = Wrap; // of Mirror of Clamp of Border
};

Texture2D m_TextureDiffuse;

//**********
// STRUCTS *
//**********
struct VS_DATA
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

//****************
// VERTEX SHADER *
//****************
VS_DATA MainVS(VS_DATA vsData)
{
    return vsData;
}

//******************
// GEOMETRY SHADER *
//******************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float3 normal, float2 texCoord)
{
	// Create the data object and zero it out
	GS_DATA data = (GS_DATA)0;

	// Apply all the transformations necessary and fill out the object
	data.Position = mul(float4(pos, 1), m_MatrixWorldViewProj);
	data.Normal = mul(normal, (float3x3) m_MatrixWorld);
	data.TexCoord = texCoord;

	// Append the vertex to the triangle stream
	triStream.Append(data);
}

[maxvertexcount(6)]
void SpikeGenerator(triangle VS_DATA vertices[3], inout TriangleStream<GS_DATA> triStream)
{
	// Constants
	const float spikeSize = 4.f;
	const float spikeWidthMultiplier = 0.1f;

	// Use these variable names
    float3 basePoint, top, left, right, spikeNormal;

	// Step 1. Calculate The basePoint
	basePoint = float3((vertices[0].Position + vertices[1].Position + vertices[2].Position) / 3.f);

	// Step 2. Calculate The normal of the basePoint
	float3 bpNormal = float3((vertices[0].Normal + vertices[1].Normal + vertices[2].Normal) / 3.f);

	// Step 3. Calculate The Spike's Top Position
	top = basePoint + (spikeSize * bpNormal);

	// Step 4. Calculate The Left And Right Positions
	float3 spikeDirection = float3(vertices[2].Position - vertices[0].Position) * spikeWidthMultiplier;
	left = basePoint - spikeDirection;
	right = basePoint + spikeDirection;

	// Step 5. Calculate The Normal of the spike
	spikeNormal = cross(left - top, right - top);

	// Step 6. Create The Vertices [Complete code in CreateVertex(...)]
	
	// Create Existing Geometry
    CreateVertex(triStream, vertices[0].Position.xyz, vertices[0].Normal, vertices[0].TexCoord);
    CreateVertex(triStream, vertices[1].Position.xyz, vertices[1].Normal, vertices[1].TexCoord);
    CreateVertex(triStream, vertices[2].Position.xyz, vertices[2].Normal, vertices[2].TexCoord);

	// Restart the strip so we can add another (independent) triangle!

    triStream.RestartStrip();

	// Create Spike Geometry
    CreateVertex(triStream, top, spikeNormal, float2(0, 0));
    CreateVertex(triStream, left, spikeNormal, float2(0, 0));
    CreateVertex(triStream, right, spikeNormal, float2(0, 0));
}

//***************
// PIXEL SHADER *
//***************
float4 MainPS(GS_DATA input) : SV_TARGET
{
    input.Normal = -normalize(input.Normal);
    float alpha = m_TextureDiffuse.Sample(samLinear, input.TexCoord).a;
    float3 color = m_TextureDiffuse.Sample(samLinear, input.TexCoord).rgb;
    float s = max(dot(m_LightDir, input.Normal), 0.4f);

    return float4(color * s, alpha);
}

//*************
// TECHNIQUES *
//*************
technique10 DefaultTechnique
{
    pass p0
    {
        SetRasterizerState(FrontCulling);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, SpikeGenerator()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}