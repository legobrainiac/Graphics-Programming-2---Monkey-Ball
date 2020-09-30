float4x4 gTransform : WorldViewProjection;
Texture2D gSpriteTexture;
float2 gTextureSize;

float4x4 gMatrixView : VIEW;
float4x4 gMatrixViewInverse : VIEWINVERSE;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

BlendState EnableBlending 
{     
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

DepthStencilState NoDepth
{
	DepthEnable = FALSE;
};

RasterizerState BackCulling 
{ 
	CullMode = BACK; 
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
	uint TextureId: TEXCOORD0;
	float4 TransformData : POSITION; //PosX, PosY, Depth (PosZ), Rotation
	float4 TransformData2 : POSITION1; //PivotX, PivotY, ScaleX, ScaleY
	float4 Color: COLOR;	
};

struct GS_DATA
{
	float4 Position : SV_POSITION;
	float4 Color: COLOR;
	float2 TexCoord: TEXCOORD0;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, float rotation, float2 rotCosSin, float2 offset, float2 pivotOffset)
{
	if (rotation != 0)
	{
		// Transform to origin tanking in to account pivot
		float2 posOrigin = (pos.xy - offset) - pivotOffset;

		// Rotate it using the rotCosSin
		posOrigin = float2 (
			(posOrigin.x * rotCosSin.x) - (posOrigin.y * rotCosSin.y), 
			(posOrigin.y * rotCosSin.x) + (posOrigin.x * rotCosSin.y)
		);

		// Retransform to initial position and apply pivot offset
		posOrigin += offset;
		pos = float3(posOrigin, pos.z);
	}
	else
	{
		//Step 2.
		//No rotation calculations (no need to do the rotation calculations if there is no rotation applied > redundant operations)
		//Just apply the pivot offset
		pos -= float3(pivotOffset, 0.f);
	}

	//Geometry Vertex Output
	GS_DATA geomData = (GS_DATA) 0;
	geomData.Position = mul(float4(pos, 1.0f), gTransform);
	geomData.Color = col;
	geomData.TexCoord = texCoord;
	triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Given Data (Vertex Data)
	float3 position = vertex[0].TransformData.xyz; //Extract the position data from the VS_DATA vertex struct
	float2 offset = vertex[0].TransformData.xy; //Extract the offset data from the VS_DATA vertex struct (initial X and Y position)
	float rotation = vertex[0].TransformData.w; //Extract the rotation data from the VS_DATA vertex struct
	float2 pivot = vertex[0].TransformData2.xy; //Extract the pivot data from the VS_DATA vertex struct
	float2 scale = vertex[0].TransformData2.zw; //Extract the scale data from the VS_DATA vertex struct
	float2 texCoord = float2(0, 0); //Initial Texture Coordinate
	float4 color = vertex[0].Color;
	
	//...

	// LT----------RT //TringleStrip (LT > RT > LB, LB > RB > RT)
	// |          / |
	// |       /    |
	// |    /       |
	// | /          |
	// LB----------RB

	// Global pivot offset
	float2 pivotOffset = pivot * scale * gTextureSize;
	float2 rotationCosSin = { 0.f, 0.f };
	
	if (rotation != 0.f)
		rotationCosSin = float2(cos(rotation), sin(rotation));	

	// VERTEX 1 [LT]
	float3 posLT = position;
	float2 uvLT = { 0.f, 0.f };

	// VERTEX 2 [RT]
	float3 posRT = position + float3(gTextureSize.x * scale.x, 0.f, 0.f);
	float2 uvRT = { 1.f, 0.f };

	// VERTEX 3 [LB]
	float3 posLB = position + float3(0.f, gTextureSize.y * scale.y, 0.f);
	float2 uvLB = { 0.f, 1.f };

	// VERTEX 4 [RB]
	float3 posRB = position + float3(gTextureSize.x * scale.x, gTextureSize.y * scale.y, 0.f);
	float2 uvRB = { 1.f, 1.f };

	CreateVertex(triStream, posLT, color, uvLT, rotation, rotationCosSin, offset, pivotOffset);
	CreateVertex(triStream, posRT, color, uvRT, rotation, rotationCosSin, offset, pivotOffset);
	CreateVertex(triStream, posLB, color, uvLB, rotation, rotationCosSin, offset, pivotOffset);
	CreateVertex(triStream, posRB, color, uvRB, rotation, rotationCosSin, offset, pivotOffset);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET 
{
	float4 color = gSpriteTexture.Sample(samPoint, input.TexCoord) * input.Color;

	clip(color.a < 0.01f ? -1 : 1); 

	return color;	
}

// Default Technique
technique11 Default {

	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		//SetDepthStencilState(NoDepth,0);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
