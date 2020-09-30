#include "stdafx.h"
#include "UberMaterial.h"

#include "ContentManager.h"
#include "TextureData.h"

// Light
ID3DX11EffectVectorVariable* UberMaterial::m_pLightDirectionVariable = nullptr;

// Diffuse
ID3DX11EffectScalarVariable* UberMaterial::m_pUseDiffuseTextureVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectVectorVariable* UberMaterial::m_pDiffuseColorVariable = nullptr;

// Specular
ID3DX11EffectVectorVariable* UberMaterial::m_pSpecularColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseSpecularLevelTextureVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pSpecularLevelSRVvariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pShininessVariable = nullptr;

// Ambient
ID3DX11EffectVectorVariable* UberMaterial::m_pAmbientColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pAmbientIntensityVariable = nullptr;

// Normal mapping
ID3DX11EffectScalarVariable* UberMaterial::m_pFlipGreenChannelVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseNormalMappingVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pNormalMappingSRVvariable = nullptr;

// Environment mapping
ID3DX11EffectScalarVariable* UberMaterial::m_pUseEnvironmentMappingVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pEnvironmentSRVvariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pReflectionStrengthVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pRefractionStrengthVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pRefractionIndexVariable = nullptr;

// Opacity
ID3DX11EffectScalarVariable* UberMaterial::m_pOpacityVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseOpacityMapVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pOpacitySRVvariable = nullptr;

// Specular models
ID3DX11EffectScalarVariable* UberMaterial::m_pUseBlinnVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUsePhongVariable = nullptr;

// Fresnel falloff
ID3DX11EffectScalarVariable* UberMaterial::m_pUseFresnelFalloffVariable = nullptr;
ID3DX11EffectVectorVariable* UberMaterial::m_pFresnelColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelPowerVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelMultiplierVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelHardnessVariable = nullptr;

UberMaterial::UberMaterial(const std::wstring& technique)
	: Material(L"./Resources/Effects/UberShader.fx", technique)
{
}

UberMaterial::~UberMaterial()
{
}

void UberMaterial::SetDiffuseTexture(const wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetSpecularLevelTexture(const wstring& assetFile)
{
	m_bSpecularLevelTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetNormalMapTexture(const wstring& assetFile)
{
	m_pNormalMappingTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetEnvironmentCube(const wstring& assetFile)
{
	m_pEnvironmentCubeTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetOpacityTexture(const wstring& assetFile)
{
	m_pOpacityMap = ContentManager::Load<TextureData>(assetFile);;
}

void UberMaterial::LoadEffectVariables()
{
	// Lights
	LOAD_SHADER_RESOURCE("gLightDirection", m_pLightDirectionVariable, AsVector);

	// Diffuse
	LOAD_SHADER_RESOURCE("gUseTextureDiffuse", m_pUseDiffuseTextureVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gColorDiffuse", m_pDiffuseColorVariable, AsVector);
	LOAD_SHADER_RESOURCE("gTextureDiffuse", m_pDiffuseSRVvariable, AsShaderResource);

	// Specular
	LOAD_SHADER_RESOURCE("gColorSpecular", m_pSpecularColorVariable, AsVector);
	LOAD_SHADER_RESOURCE("gUseTextureSpecularIntensity", m_pUseSpecularLevelTextureVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gTextureSpecularIntensity", m_pSpecularLevelSRVvariable, AsShaderResource);
	LOAD_SHADER_RESOURCE("gShininess", m_pShininessVariable, AsScalar);

	// Ambient
	LOAD_SHADER_RESOURCE("gColorAmbient", m_pAmbientColorVariable, AsVector);
	LOAD_SHADER_RESOURCE("gAmbientIntensity", m_pAmbientIntensityVariable, AsScalar);

	// Normal mapping
	LOAD_SHADER_RESOURCE("gFlipGreenChannel", m_pFlipGreenChannelVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gUseTextureNormal", m_pUseNormalMappingVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gTextureNormal", m_pNormalMappingSRVvariable, AsShaderResource);

	// Environment mapping
	LOAD_SHADER_RESOURCE("gUseTextureEnvironment", m_pUseEnvironmentMappingVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gCubeEnvironment", m_pEnvironmentSRVvariable, AsShaderResource);
	LOAD_SHADER_RESOURCE("gReflectionStrength", m_pReflectionStrengthVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gRefractionStrength", m_pRefractionStrengthVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gRefractionIndex", m_pRefractionIndexVariable, AsScalar);

	// Opacity
	LOAD_SHADER_RESOURCE("gOpacityIntensity", m_pOpacityVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gUseTextureOpacity", m_pUseOpacityMapVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gTextureOpacity", m_pOpacitySRVvariable, AsShaderResource);

	// Specular models
	LOAD_SHADER_RESOURCE("gUseSpecularBlinn", m_pUseBlinnVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gUseSpecularPhong", m_pUsePhongVariable, AsScalar);

	// Fresnel falloff
	LOAD_SHADER_RESOURCE("gUseFresnelFalloff", m_pUseFresnelFalloffVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gColorFresnel", m_pFresnelColorVariable, AsVector);
	LOAD_SHADER_RESOURCE("gFresnelPower", m_pFresnelPowerVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gFresnelMultiplier", m_pFresnelMultiplierVariable, AsScalar);
	LOAD_SHADER_RESOURCE("gFresnelHardness", m_pFresnelHardnessVariable, AsScalar);
}

void UberMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	(void)gameContext;
	(void)pModelComponent;

	// Light
	if (m_pLightDirectionVariable)
		m_pLightDirectionVariable->SetFloatVector(&m_LightDirection.x);

	// Diffuse
	if (m_pUseDiffuseTextureVariable)
		m_pUseDiffuseTextureVariable->SetBool(m_bDiffuseTexture);

	if (m_pDiffuseTexture && m_pDiffuseSRVvariable)
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

	if (m_pDiffuseColorVariable)
		m_pDiffuseColorVariable->SetFloatVector(&m_ColorDiffuse.x);

	// Specular
	if (m_pSpecularColorVariable)
		m_pSpecularColorVariable->SetFloatVector(&m_ColorSpecular.x);

	if (m_pUseSpecularLevelTextureVariable)
		m_pUseSpecularLevelTextureVariable->SetBool(m_bSpecularLevelTexture);

	if (m_pSpecularLevelTexture && m_pSpecularLevelSRVvariable)
		m_pSpecularLevelSRVvariable->SetResource(m_pSpecularLevelTexture->GetShaderResourceView());

	if (m_pShininessVariable)
		m_pShininessVariable->SetInt(m_Shininess);

	// Ambient
	if (m_pAmbientColorVariable)
		m_pAmbientColorVariable->SetFloatVector(&m_ColorAmbient.x);

	if (m_pAmbientIntensityVariable)
		m_pAmbientIntensityVariable->SetFloat(m_AmbientIntensity);

	// Normal mapping
	if (m_pFlipGreenChannelVariable)
		m_pFlipGreenChannelVariable->SetBool(m_bFlipGreenChannel);

	if (m_pUseNormalMappingVariable)
		m_pUseNormalMappingVariable->SetBool(m_bNormalMapping);

	if (m_pNormalMappingTexture && m_pNormalMappingSRVvariable)
		m_pNormalMappingSRVvariable->SetResource(m_pNormalMappingTexture->GetShaderResourceView());

	// Environment mapping
	if (m_pUseEnvironmentMappingVariable)
		m_pUseEnvironmentMappingVariable->SetBool(m_bEnvironmentMapping);

	if (m_pEnvironmentCubeTexture && m_pEnvironmentSRVvariable)
		m_pEnvironmentSRVvariable->SetResource(m_pEnvironmentCubeTexture->GetShaderResourceView());

	if (m_pReflectionStrengthVariable)
		m_pReflectionStrengthVariable->SetFloat(m_ReflectionStrength);

	if (m_pRefractionStrengthVariable)
		m_pRefractionStrengthVariable->SetFloat(m_RefractionStrength);

	if (m_pRefractionIndexVariable)
		m_pRefractionIndexVariable->SetFloat(m_RefractionIndex);

	// Opacity
	if (m_pOpacityVariable)
		m_pOpacityVariable->SetFloat(m_Opacity);

	if (m_pUseOpacityMapVariable)
		m_pUseOpacityMapVariable->SetBool(m_bOpacityMap);

	if (m_pOpacityMap && m_pOpacitySRVvariable)
		m_pOpacitySRVvariable->SetResource(m_pOpacityMap->GetShaderResourceView());

	// Specular models
	if (m_pUseBlinnVariable)
		m_pUseBlinnVariable->SetBool(m_bSpecularBlinn);

	if (m_pUsePhongVariable)
		m_pUsePhongVariable->SetBool(m_bSpecularPhong);

	// Fresnel falloff
	if (m_pUseFresnelFalloffVariable)
		m_pUseFresnelFalloffVariable->SetBool(m_bFresnelFaloff);

	if (m_pFresnelColorVariable)
		m_pFresnelColorVariable->SetFloatVector(&m_ColorFresnel.x);

	if (m_pFresnelPowerVariable)
		m_pFresnelPowerVariable->SetFloat(m_FresnelPower);

	if (m_pFresnelMultiplierVariable)
		m_pFresnelMultiplierVariable->SetFloat(m_FresnelMultiplier);

	if (m_pFresnelHardnessVariable)
		m_pFresnelHardnessVariable->SetFloat(m_FresnelHardness);
}
