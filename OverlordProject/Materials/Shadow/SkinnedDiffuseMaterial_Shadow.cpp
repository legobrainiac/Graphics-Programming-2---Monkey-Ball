//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SkinnedDiffuseMaterial_Shadow.h"
#include "GeneralStructs.h"
#include "Logger.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "ModelComponent.h"
#include "ModelAnimator.h"
#include "Components.h"

ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial_Shadow::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial_Shadow::m_pBoneTransforms = nullptr;
ID3DX11EffectVectorVariable* SkinnedDiffuseMaterial_Shadow::m_pLightDirectionVariable = nullptr;
ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial_Shadow::m_pShadowSRVvariable = nullptr;
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial_Shadow::m_pLightWVPvariable = nullptr;

SkinnedDiffuseMaterial_Shadow::SkinnedDiffuseMaterial_Shadow() : Material(L"./Resources/Effects/Shadow/PosNormTex3D_Skinned_Shadow.fx"),
	m_pDiffuseTexture(nullptr)
{}

void SkinnedDiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	UNREFERENCED_PARAMETER(assetFile);

	//TODO: store the diffuse texture in the appropriate member
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void SkinnedDiffuseMaterial_Shadow::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	UNREFERENCED_PARAMETER(dir);

	//TODO: store the light direction in the appropriate member
	m_LightDirection = dir;
}

void SkinnedDiffuseMaterial_Shadow::LoadEffectVariables()
{
	//TODO: load all the necessary shader variables
	const auto VariableByName = [this](const char* name) -> ID3DX11EffectVariable *
	{
		auto effectVariable = GetEffect()->GetVariableByName(name);
		return (effectVariable->IsValid()) ? effectVariable : nullptr;
	};

	m_pDiffuseSRVvariable = VariableByName("gDiffuseMap")->AsShaderResource();
	m_pBoneTransforms = VariableByName("gBones")->AsMatrix();
	m_pShadowSRVvariable = VariableByName("gShadowMap")->AsShaderResource();
	m_pLightDirectionVariable = VariableByName("gDiffuseMap")->AsVector();
	m_pLightWVPvariable = VariableByName("gWorldViewProj_Light")->AsMatrix();
}

void SkinnedDiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	if (m_pDiffuseSRVvariable)
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

	if (m_pBoneTransforms)
	{
		auto bones = pModelComponent->GetAnimator()->GetBoneTransforms();
		m_pBoneTransforms->SetMatrixArray(reinterpret_cast<float*>(&bones[0]), 0, bones.size());
	}

	if (m_pShadowSRVvariable)
		m_pShadowSRVvariable->SetResource(gameContext.pShadowMapper->GetShadowMap());

	if (m_pLightDirectionVariable)
		m_pLightDirectionVariable->SetFloatVector(reinterpret_cast<float*>(&m_LightDirection));

	if (m_pLightWVPvariable)
	{
		auto world = XMLoadFloat4x4(&pModelComponent->GetTransform()->GetWorld());
		auto lvp = gameContext.pShadowMapper->GetLightVP();
		auto lWvp = world * XMLoadFloat4x4(&lvp);
		m_pLightWVPvariable->SetMatrix(reinterpret_cast<float*>(&lWvp));
	}
}