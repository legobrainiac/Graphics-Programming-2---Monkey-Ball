//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ShadowMapMaterial.h"
#include "ContentManager.h"

ShadowMapMaterial::~ShadowMapMaterial()
{
	// Release techniques
	for (uint32_t i = 0U; i < 2U; ++i)
		SafeRelease(m_pInputLayouts[i]);
}

void ShadowMapMaterial::Initialize(const GameContext& gameContext)
{
	constexpr auto filePath = L"./Resources/Effects/ShadowMapGenerator.fx";
	constexpr char* techniqueNames[2] { "GenerateShadows", "GenerateShadows_Skinned" };

	if (!m_IsInitialized)
	{
		m_pShadowEffect = ContentManager::Load<ID3DX11Effect>(filePath);

		// Load effect
		if (m_pShadowEffect)
		{
			// Load techniques, Not validated, i know... :P
			m_pShadowTechs[ShadowGenType::Static] = m_pShadowEffect->GetTechniqueByName(techniqueNames[0]);
			m_pShadowTechs[ShadowGenType::Skinned] = m_pShadowEffect->GetTechniqueByName(techniqueNames[1]);
		}

		// Build the input layouts using le fancy function :)
		for (uint32_t i = 0U; i < 2U; ++i)
		{
			EffectHelper::BuildInputLayout(
				gameContext.pDevice,
				m_pShadowTechs[i],
				&m_pInputLayouts[i],
				m_InputLayoutDescriptions[i],
				m_InputLayoutSizes[i],
				m_InputLayoutIds[i]);
		}

		// Shader resource variables
		const auto MatrixVariableByName = [this](const char* name) -> ID3DX11EffectMatrixVariable*
		{
			auto effectVariable = m_pShadowEffect->GetVariableByName(name)->AsMatrix();
			return (effectVariable->IsValid()) ? effectVariable : nullptr;
		};

		m_pWorldMatrixVariable = MatrixVariableByName("gWorld");
		m_pLightVPMatrixVariable = MatrixVariableByName("gLightViewProj");
		m_pBoneTransforms = MatrixVariableByName("gBones");
	}
}

void ShadowMapMaterial::SetLightVP(DirectX::XMFLOAT4X4 lightVP) const
{
	if (m_pLightVPMatrixVariable)
		m_pLightVPMatrixVariable->SetMatrix(reinterpret_cast<float*>(&lightVP));
}

void ShadowMapMaterial::SetWorld(DirectX::XMFLOAT4X4 world) const
{
	if (m_pWorldMatrixVariable)
		m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<float*>(&world));
}

void ShadowMapMaterial::SetBones(const float* pData, int count) const
{
	if (m_pBoneTransforms)
		m_pBoneTransforms->SetMatrixArray(pData, 0, count);
}
