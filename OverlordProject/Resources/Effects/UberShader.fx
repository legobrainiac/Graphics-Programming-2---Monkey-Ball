/*
******************
* DAE Ubershader *
******************

**This Shader Contains:

- Diffuse (Texture & Color)
	- Regular Diffuse
- Specular
	- Specular Level (Texture & Value)
	- Shininess (Value)
	- Models
		- Blinn
		- Phong
- Ambient (Color)
- EnvironmentMapping (CubeMap)
	- Reflection + Fresnel Falloff
	- Refraction
- Normal (Texture)
- Opacity (Texture & Value)

-Techniques
	- WithAlphaBlending
	- WithoutAlphaBlending
*/

//GLOBAL MATRICES
//***************
// The World View Projection Matrix
float4x4 gMatrixWVP : WORLDVIEWPROJECTION;
// The ViewInverse Matrix - the third row contains the camera position!
float4x4 gMatrixViewInverse : VIEWINVERSE;
// The World Matrix
float4x4 gMatrixWorld : WORLD;
float4x4 gMatrixView : VIEW;

//STATES
//******
RasterizerState gRS_FrontCulling 
{ 
	CullMode = NONE; 
};

BlendState gBS_NoBlending
{
};

BlendState gBS_EnableBlending 
{     
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

//SAMPLER STATES
//**************
SamplerState gTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
 	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
};

//LIGHT
//*****
float3 gLightDirection : DIRECTION
<
	string UIName = "Light Direction";
	string Object = "TargetLight";
> = float3(0.577f, 0.577f, 0.577f);

//DIFFUSE
//*******
bool gUseTextureDiffuse
<
	string UIName = "Use Diffuse Texture?";
	string UIWidget = "Bool";
> = false;

float4 gColorDiffuse
<
	string UIName = "Diffuse Color";
	string UIWidget = "Color";
> = float4(1,1,1,1);

Texture2D gTextureDiffuse
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Texture";
	string ResourceName = "CobbleStone_DiffuseMap.dds";
>;

//SPECULAR
//********
float4 gColorSpecular
<
	string UIName = "Specular Color";
	string UIWidget = "Color";
> = float4(1,1,1,1);

Texture2D gTextureSpecularIntensity
<
	string UIName = "Specular Intensity Texture";
	string UIWidget = "Texture";
	string ResourceName = "CobbleStone_HeightMap.dds";
>;

bool gUseTextureSpecularIntensity
<
	string UIName = "Use Specular Level Texture?";
	string UIWidget = "Bool";
> = false;

int gShininess
<
	string UIName = "Shininess";
	string UIWidget = "Slider";
	float UIMin = 1;
	float UIMax = 100;
	float UIStep = 0.01;
> = 15;

//AMBIENT
//*******
float4 gColorAmbient
<
	string UIName = "Ambient Color";
	string UIWidget = "Color";
> = float4(0,0,0,1);

float gAmbientIntensity
<
	string UIName = "Ambient Intensity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
	float UIStep = 0.01;
>  = 0.0f;

//NORMAL MAPPING
//**************
bool gFlipGreenChannel
<
	string UIName = "Flip Green Channel";
	string UIWidget = "Bool";
> = false;

bool gUseTextureNormal
<
	string UIName = "Use Normal Texture?";
	string UIWidget = "Bool";
> = false;

Texture2D gTextureNormal
<
	string UIName = "Normal Texture";
	string UIWidget = "Texture";
	string ResourceName = "CobbleStone_NormalMap.dds";
>;

//ENVIRONMENT MAPPING
//*******************
TextureCube gCubeEnvironment
<
	string UIName = "Environment Cube";
	string ResourceType = "Cube";
	string ResourceName = "Sunol_Cubemap.dds";
>;

bool gUseTextureEnvironment
<
	string UIName = "Use Environment Texture?";
	string UIWidget = "Bool";
> = false;

float gReflectionStrength
<
	string UIName = "Reflection Strength";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
	float UIStep = 0.01;
>  = 0.0f;

float gRefractionStrength
<
	string UIName = "Refraction Strength";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
	float UIStep = 0.01;
>  = 0.0f;

float gRefractionIndex
<
	string UIName = "Refraction Index";
>  = 0.3f;

//FRESNEL FALLOFF
//***************
bool gUseFresnelFalloff
<
	string UIName = "Use Fresnel FallOff?";
	string UIWidget = "Bool";
> = false;


float4 gColorFresnel
<
	string UIName = "Fresnel Color";
	string UIWidget = "Color";
> = float4(1,1,1,1);

float gFresnelPower
<
	string UIName = "Fresnel Power";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 50;
	float UIStep = 0.01;
>  = 1.0f;

float gFresnelMultiplier
<
	string UIName = "Fresnel Multiplier";
	string UIWidget = "slider";
	float UIMin = 1;
	float UIMax = 100;
	float UIStep = 0.01;
>  = 1.0;

float gFresnelHardness
<
	string UIName = "Fresnel Hardness";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 50;
	float UIStep = 0.01;
>  = 0;

//OPACITY
//***************
float gOpacityIntensity
<
	string UIName = "Opacity Intensity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
	float UIStep = 0.01;
>  = 1.0f;

bool gUseTextureOpacity
<
	string UIName = "Use Opacity Texture?";
	string UIWidget = "Bool";
> = false;

Texture2D gTextureOpacity
<
	string UIName = "Opacity Map";
	string UIWidget = "Texture";
	string ResourceName = "Specular_Level.jpg";
>;


//SPECULAR MODELS
//***************
bool gUseSpecularBlinn
<
	string UIName = "Use Specular Blinn?";
	string UIWidget = "Bool";
> = false;

bool gUseSpecularPhong
<
	string UIName = "Use Specular Phong?";
	string UIWidget = "Bool";
> = false;

//VS IN & OUT
//***********
struct VS_Input
{
	float3 Position: POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

struct VS_Output
{
	float4 Position: SV_POSITION;
	float4 WorldPosition: COLOR0;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

//DIFFUSE FUNCTION
float3 CalculateDiffuse(float3 normal, float2 texCoord)
{
	float3 diffuseColor = gColorDiffuse;

	// Half lambert
	float lc = pow(((dot(normal, -gLightDirection) * 0.5f) + 0.5f), 2.f);
	
	if(gUseTextureDiffuse)
	{
		float4 sampledColour = gTextureDiffuse.Sample(gTextureSampler, texCoord);
		return sampledColour.rgb * lc;
	}
	else
		return diffuseColor * lc;
	
}

//SPECULAR FUNCTION (BLINN)
float3 CalculateSpecularBlinn(float3 viewDirection, float3 normal, float2 texCoord)
{
	float intensity = 1.f;
	float3 specularColor = float3(0.0f, 0.0f, 0.0f);
	
	if(gUseTextureSpecularIntensity)
		intensity = gTextureSpecularIntensity.Sample(gTextureSampler, texCoord).r;
	
	float3 halfVector = normalize(viewDirection + gLightDirection);
	float specularStrength = dot(-halfVector, normal);
	specularStrength = saturate(specularStrength);
	specularStrength = pow(specularStrength, gShininess);
		
	specularColor = intensity * gColorSpecular * specularStrength;
	return specularColor;
}

//SPECULAR FUNCTION (PHONG)
float3 CalculateSpecularPhong(float3 viewDirection, float3 normal, float2 texCoord)
{
	float intensity = 1.f;
	float3 specularColor = float3(0.0f, 0.0f, 0.0f);
	
	if(gUseTextureSpecularIntensity)
		intensity = gTextureSpecularIntensity.Sample(gTextureSampler, texCoord).r;
	
	float3 reflectedVector = reflect(-gLightDirection, normal);
	float specularStrength = dot(viewDirection, reflectedVector);
	specularStrength = saturate(specularStrength);
	specularStrength = pow(specularStrength, gShininess);
		
	specularColor = intensity * gColorSpecular * specularStrength;
	return specularColor;
}

//SPECULAR FUNCTION (SHARED) - finished
float3 CalculateSpecular(float3 viewDirection, float3 normal, float2 texCoord)
{
	float3 specularColor = float3(0,0,0);
	
	if(gUseSpecularBlinn)
		specularColor += CalculateSpecularBlinn(viewDirection, normal, texCoord);
		
	if(gUseSpecularPhong)
		specularColor += CalculateSpecularPhong(viewDirection, normal, texCoord);
				
	return specularColor;
}

//NORMAL MAPPING FUNCTION
float3 CalculateNormal(float3 tangent, float3 normal, float2 texCoord)
{
	float3 newNormal = normal;
	
	if(gUseTextureNormal)
	{
		float3 binormal = cross(normal, tangent);
		
		if(gFlipGreenChannel)
			binormal = -binormal;
		
		float3x3 localAxis = float3x3(tangent, binormal, normal);
		float3 sampledNormal = gTextureNormal.Sample(gTextureSampler, texCoord);
		newNormal = 2.f * sampledNormal - float3(1.f, 1.f, 1.f);
		newNormal = mul(newNormal, localAxis);
		newNormal = normalize(newNormal);
	}
	
	return newNormal;
}

//FRESNEL FALLOFF FUNCTION
float3 CalculateFresnelFalloff(float3 normal, float3 viewDirection, float3 environmentColor)
{
	if(gUseFresnelFalloff)
	{
		// Fresnel
		float fresnelDotProd = 1 - saturate(abs(dot(normal, viewDirection)));
		float fresnel = pow(clamp(fresnelDotProd, 0.f, 1.f), gFresnelPower) * gFresnelMultiplier;
		
		// Fresnel mask
		float fresnelMaskDotProd = dot(float3(0, -1, 0), normal);
		float fresnelMask = pow(1 - saturate(clamp(fresnelMaskDotProd, 0.f, 1.f)), gFresnelHardness);

		float finalFresnel = (fresnel * fresnelMask);
		
		if(gUseTextureEnvironment)
			return environmentColor * finalFresnel;
		
		return gColorFresnel * finalFresnel;
	}
	
	return environmentColor;
}


//ENVIRONMENT MAPPING FUNCTION
float3 CalculateEnvironment(float3 viewDirection, float3 normal)
{
	float3 environmentColor = float3(0,0,0);
	
	if(gUseTextureEnvironment)
	{
		float3 reflection = reflect(viewDirection, normal);
		float3 refraction = refract(viewDirection, normal, gRefractionIndex);
	
	
		environmentColor += gCubeEnvironment.Sample(gTextureSampler, reflection) * gReflectionStrength;
		environmentColor += gCubeEnvironment.Sample(gTextureSampler, refraction) * gRefractionStrength;
	}
	
	return saturate(environmentColor);
}

//OPACITY FUNCTION
float CalculateOpacity(float2 texCoord)
{
	float opacity = gOpacityIntensity;
	
	if(gUseTextureOpacity)
		opacity = gTextureOpacity.Sample(gTextureSampler, texCoord).r;
	
	return opacity;
}

//MAIN VERTEX SHADER
//******************
VS_Output MainVS(VS_Input input) {
	
	VS_Output output = (VS_Output)0;
	
	output.Position = mul(float4(input.Position, 1.0), gMatrixWVP);
	output.WorldPosition = mul(float4(input.Position,1.0), gMatrixWorld);
	output.Normal = mul(input.Normal, (float3x3)gMatrixWorld);
	output.Tangent = mul(input.Tangent, (float3x3)gMatrixWorld);
	output.TexCoord = input.TexCoord;
	
	return output;
}

//MAIN PIXEL SHADER
//*****************
float4 MainPS(VS_Output input) : SV_TARGET 
{
	// NORMALIZE
	input.Normal = normalize(input.Normal);
	input.Tangent = normalize(input.Tangent);
	
	float3 viewDirection = normalize(input.WorldPosition.xyz - gMatrixViewInverse[3].xyz);
	
	//NORMAL
	float3 newNormal = CalculateNormal(input.Tangent, input.Normal, input.TexCoord);
		
	//SPECULAR
	float3 specColor = CalculateSpecular(viewDirection, newNormal, input.TexCoord);
		
	//DIFFUSE
	float3 diffColor = CalculateDiffuse(newNormal, input.TexCoord);
		
	//AMBIENT
	float3 ambientColor = gColorAmbient * gAmbientIntensity;
		
	//ENVIRONMENT MAPPING
	float3 environmentColor = CalculateEnvironment(viewDirection, newNormal);
	
	//FRESNEL FALLOFF
	environmentColor = CalculateFresnelFalloff(newNormal, viewDirection, environmentColor);
		
	//FINAL COLOR CALCULATION
	float3 finalColor = diffColor + specColor + environmentColor + ambientColor;
	
	//OPACITY
	float opacity = CalculateOpacity(input.TexCoord);
	
	return float4(finalColor, opacity);
}

// Default Technique
technique11 WithAlphaBlending 
{
	pass p0 
	{
		SetRasterizerState(gRS_FrontCulling);
		SetBlendState(gBS_EnableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, MainVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, MainPS()));
	}
}

// Default Technique
technique11 WithoutAlphaBlending 
{
	pass p0 
	{
		SetRasterizerState(gRS_FrontCulling);
		SetBlendState(gBS_NoBlending, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, MainVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, MainPS()));
	}
}