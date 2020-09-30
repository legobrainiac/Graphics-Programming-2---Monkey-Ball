//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "DiffuseMaterial_Shadow.h"
#include "GeneralStructs.h"
#include "Logger.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "Components.h"

ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow::m_pShadowSRVvariable = nullptr;
ID3DX11EffectVectorVariable* DiffuseMaterial_Shadow::m_pLightDirectionVariable = nullptr;
ID3DX11EffectMatrixVariable* DiffuseMaterial_Shadow::m_pLightWVPvariable = nullptr;
ID3DX11EffectScalarVariable* DiffuseMaterial_Shadow::m_pElapsedVariable = nullptr;

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow(bool pannedTexture) 
	: Material(pannedTexture ? L"./Resources/Effects/Shadow/PosNormTex3D_ShadowPannedTex.fx" : L"./Resources/Effects/Shadow/PosNormTex3D_Shadow.fx")
	, m_pDiffuseTexture(nullptr)
{
}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	UNREFERENCED_PARAMETER(assetFile);
	
	//TODO: store the diffuse texture in the appropriate member
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void DiffuseMaterial_Shadow::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	//TODO: store the light direction in the appropriate member
	m_LightDirection = dir;
}

void DiffuseMaterial_Shadow::LoadEffectVariables()
{
	//TODO: load all the necessary shader variables
	const auto VariableByName = [this](const char* name) -> ID3DX11EffectVariable*
	{
		auto effectVariable = GetEffect()->GetVariableByName(name);
		
		if (!effectVariable)
			return nullptr;

		return (effectVariable->IsValid()) ? effectVariable : nullptr;
	};

	m_pDiffuseSRVvariable = VariableByName("gDiffuseMap")->AsShaderResource();
	m_pShadowSRVvariable = VariableByName("gShadowMap")->AsShaderResource();
	m_pLightDirectionVariable = VariableByName("gDiffuseMap")->AsVector();
	m_pLightWVPvariable = VariableByName("gWorldViewProj_Light")->AsMatrix();

	auto varTE = VariableByName("gTotalElapsed");
	m_pElapsedVariable = (varTE) ? varTE->AsScalar() : nullptr;
}

void DiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(pModelComponent);

	if (m_pDiffuseSRVvariable)
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

	if (m_pShadowSRVvariable)
		m_pShadowSRVvariable->SetResource(gameContext.pShadowMapper->GetShadowMap());

	if (m_pLightDirectionVariable)
		m_pLightDirectionVariable->SetFloatVector(reinterpret_cast<const float*>(&m_LightDirection));

	if (m_pElapsedVariable)
		m_pElapsedVariable->SetFloat(gameContext.pGameTime->GetTotal());

	if (m_pLightWVPvariable)
	{
		auto world = XMLoadFloat4x4(&pModelComponent->GetTransform()->GetWorld());
		auto lvp = gameContext.pShadowMapper->GetLightVP();
		auto lWvp = world * XMLoadFloat4x4(&lvp);
		m_pLightWVPvariable->SetMatrix(reinterpret_cast<float*>(&lWvp));
	}
}