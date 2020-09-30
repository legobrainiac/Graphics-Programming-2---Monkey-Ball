#pragma once

#include "Material.h"
#include <string>

class TextureData;

using namespace std;
using namespace DirectX;

#define LOAD_SHADER_RESOURCE(CSTR_SRVN, PTR_TARGET_SRV, TECHNIQUE)\
if(!PTR_TARGET_SRV)\
{\
	PTR_TARGET_SRV = GetEffect()->GetVariableByName(CSTR_SRVN)->TECHNIQUE();\
	if(!PTR_TARGET_SRV->IsValid())\
	{\
		PTR_TARGET_SRV = nullptr;\
	}\
}\

#define DXRELEASE(P_OBJ) if(P_OBJ) { P_OBJ->Release(); P_OBJ = nullptr; }

class UberMaterial
	: public Material
{
public:
	UberMaterial(const std::wstring& technique = L"WithoutAlphaBlending");
	~UberMaterial();

	//LIGHT
	//*****
	void SetLightDirection(XMFLOAT3 direction) { m_LightDirection = direction; }

	//DIFFUSE
	//*******
	void EnableDiffuseTexture(bool enable) { m_bDiffuseTexture = enable; }
	void SetDiffuseTexture(const wstring& assetFile);
	void SetDiffuseColor(XMFLOAT4 color) { m_ColorDiffuse = color; }

	//SPECULAR
	//********
	void SetSpecularColor(XMFLOAT4 color) { m_ColorSpecular = color; }
	void EnableSpecularLevelTexture(bool enable) { m_bSpecularLevelTexture = enable; }
	void SetSpecularLevelTexture(const wstring& assetFile);
	void SetShininess(int shininess) { m_Shininess = shininess; }

	//AMBIENT
	//*******
	void SetAmbientColor(XMFLOAT4 color) { m_ColorAmbient = color; }
	void SetAmbientIntensity(float intensity) { m_AmbientIntensity = intensity; }

	//NORMAL MAPPING
	//**************
	void FlipNormalGreenCHannel(bool flip) { m_bFlipGreenChannel = flip; }
	void EnableNormalMapping(bool enable) { m_bNormalMapping = enable; }
	void SetNormalMapTexture(const wstring& assetFile);

	//ENVIRONMENT MAPPING
	//*******************
	void EnableEnvironmentMapping(bool enable) { m_bEnvironmentMapping = enable; }
	void SetEnvironmentCube(const wstring& assetFile);
	void SetReflectionStrength(float strength) { m_ReflectionStrength = strength; }
	void SetRefractionStrength(float strength) { m_RefractionStrength = strength; }
	void SetRefractionIndex(float index) { m_RefractionIndex = index; }

	//OPACITY
	//*******
	void SetOpacity(float opacity) { m_Opacity = opacity; }
	void EnableOpacityMap(bool enable) { m_bOpacityMap = enable; }
	void SetOpacityTexture(const wstring& assetFile);

	//SPECULAR MODELS
	//***************
	void EnableSpecularBlinn(bool enable) { m_bSpecularBlinn = enable; }
	void EnableSpecularPhong(bool enable) { m_bSpecularPhong = enable; }

	//FRESNEL FALLOFF
	//***************
	void EnableFresnelFaloff(bool enable) { m_bFresnelFaloff = enable; }
	void SetFresnelColor(XMFLOAT4 color) { m_ColorFresnel = color; }
	void SetFresnelPower(float power) { m_FresnelPower = power; }
	void SetFresnelMultiplier(float multiplier) { m_FresnelMultiplier = multiplier; }
	void SetFresnelHardness(float hardness) { m_FresnelHardness = hardness; }

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent*
		pModelComponent);

private:
	//LIGHT
	//*****
	XMFLOAT3 m_LightDirection = XMFLOAT3{ 0.577f, 0.577f, 0.577f };
	static ID3DX11EffectVectorVariable* m_pLightDirectionVariable;

	//DIFFUSE
	//*******
	bool m_bDiffuseTexture = false;
	static ID3DX11EffectScalarVariable* m_pUseDiffuseTextureVariable;
	TextureData* m_pDiffuseTexture = nullptr;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	XMFLOAT4 m_ColorDiffuse = XMFLOAT4{ 1.f, 1.f, 1.f, 1.f };
	static ID3DX11EffectVectorVariable* m_pDiffuseColorVariable;

	//SPECULAR
	//********
	XMFLOAT4 m_ColorSpecular = XMFLOAT4{ 1.f, 1.f, 1.f, 1.f };
	static ID3DX11EffectVectorVariable* m_pSpecularColorVariable;
	bool m_bSpecularLevelTexture = false;
	static ID3DX11EffectScalarVariable* m_pUseSpecularLevelTextureVariable;
	TextureData* m_pSpecularLevelTexture = nullptr;
	static ID3DX11EffectShaderResourceVariable* m_pSpecularLevelSRVvariable;
	int m_Shininess = 15;
	static ID3DX11EffectScalarVariable* m_pShininessVariable;

	//AMBIENT
	//*******
	XMFLOAT4 m_ColorAmbient = XMFLOAT4{ 0.f, 0.f, 0.f, 1.f };;
	static ID3DX11EffectVectorVariable* m_pAmbientColorVariable;
	float m_AmbientIntensity = 0.f;
	static ID3DX11EffectScalarVariable* m_pAmbientIntensityVariable;

	//NORMAL MAPPING
	//**************
	bool m_bFlipGreenChannel = false;
	static ID3DX11EffectScalarVariable* m_pFlipGreenChannelVariable;
	bool m_bNormalMapping = false;
	static ID3DX11EffectScalarVariable* m_pUseNormalMappingVariable;
	TextureData* m_pNormalMappingTexture = nullptr;
	static ID3DX11EffectShaderResourceVariable* m_pNormalMappingSRVvariable;

	//ENVIRONMENT MAPPING
	//*******************
	bool m_bEnvironmentMapping = false;
	static ID3DX11EffectScalarVariable* m_pUseEnvironmentMappingVariable;
	TextureData* m_pEnvironmentCubeTexture = nullptr;
	static ID3DX11EffectShaderResourceVariable* m_pEnvironmentSRVvariable;
	float m_ReflectionStrength = 0.f;
	static ID3DX11EffectScalarVariable* m_pReflectionStrengthVariable;
	float m_RefractionStrength = 0.f;
	static ID3DX11EffectScalarVariable* m_pRefractionStrengthVariable;
	float m_RefractionIndex = 0.3f;
	static ID3DX11EffectScalarVariable* m_pRefractionIndexVariable;

	//OPACITY
	//***************
	float m_Opacity = 0.f;
	static ID3DX11EffectScalarVariable* m_pOpacityVariable;
	bool m_bOpacityMap = false;
	static ID3DX11EffectScalarVariable* m_pUseOpacityMapVariable;
	TextureData* m_pOpacityMap = nullptr;
	static ID3DX11EffectShaderResourceVariable* m_pOpacitySRVvariable;

	//SPECULAR MODELS
	//***************
	bool m_bSpecularBlinn = false;
	static ID3DX11EffectScalarVariable* m_pUseBlinnVariable;
	bool m_bSpecularPhong = false;
	static ID3DX11EffectScalarVariable* m_pUsePhongVariable;

	//FRESNEL FALLOFF
	//***************
	bool m_bFresnelFaloff = false;
	static ID3DX11EffectScalarVariable* m_pUseFresnelFalloffVariable;
	XMFLOAT4 m_ColorFresnel = XMFLOAT4{ 1.f, 1.f, 1.f, 1.f };
	static ID3DX11EffectVectorVariable* m_pFresnelColorVariable;
	float m_FresnelPower = 1.f;
	static ID3DX11EffectScalarVariable* m_pFresnelPowerVariable;
	float m_FresnelMultiplier = 1.f;
	static ID3DX11EffectScalarVariable* m_pFresnelMultiplierVariable;
	float m_FresnelHardness = 0;
	static ID3DX11EffectScalarVariable* m_pFresnelHardnessVariable;

private:
	// -------------------------
	// Disabling default copy constructor and default
	// assignment operator.
	// -------------------------
	UberMaterial(const UberMaterial& obj);
	UberMaterial& operator=(const UberMaterial& obj);
};