//=============================================================================
//// Shader uses position and texture
//=============================================================================

float4x4 gMatrixView : VIEW;
float4x4 gMatrixViewInverse : VIEWINVERSE;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
/// Create Rasterizer State (Backface culling) 
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
	// Set the Position
	output.Position = float4(input.Position, 1.f);

	// Set the TexCoord
	output.TexCoord = input.TexCoord;

	return output;
}

//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	// Step 1: find the dimensions of the texture (the texture has a method for that)	
	float width, height;
	gTexture.GetDimensions(width, height);

	// Step 2: calculate dx and dy (UV space for 1 pixel)	
	float dx = 1.f / width;
	float dy = 1.f / height;
	float2 uv = input.TexCoord;

	float4 finalColour;

	// Step 3: Create a double for loop (5 iterations each)
	for(int x = 0; x < 5; x++)
	{
		for(int y = 0; y < 5; y++)
		{
			// Inside the loop, calculate the offset in each direction. Make sure not to take every pixel but move by 2 pixels each time
			float2 sampleUv = float2((x * 2.f * dx) + uv.x, (y * 2.f * dy) + uv.y);

			// Do a texture lookup using your previously calculated uv coordinates + the offset, and add to the final color
			finalColour += gTexture.Sample(samPoint, sampleUv);
		}
	}
	
	// Step 4: Divide the final color by the number of passes (in this case 5*5)	
	finalColour /= 5 * 5;
	
	// Step 5: return the final color
	return finalColour;
}


//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
		// Set states...
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
       	SetRasterizerState(BackCulling);       
		SetDepthStencilState(DefaultDepthStencilState, 0);
    }
}