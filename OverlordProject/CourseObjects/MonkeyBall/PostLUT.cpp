#include "stdafx.h"
#include "PostLUT.h"
#include "RenderTarget.h"
#include "ContentManager.h"

#include "ContentManager.h"
#include "TextureData.h"

PostLUT::PostLUT()
	: PostProcessingMaterial(L"./Resources/Effects/Post/LUT.fx", 1),
	m_pTextureMapVariabele(nullptr)
{
	m_pLutTexture = ContentManager::Load<TextureData>(L"Resources/MonkeyBall/Texture/LUT/blueish.png");
}

void PostLUT::LoadEffectVariables()
{
	//TODO: Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	//Check if valid!
	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariabele->IsValid())
		Logger::LogError(L"PostGrayscale::LoadEffectVariables() => failed to get shader variable gTexture ");

	m_pLut = GetEffect()->GetVariableByName("gLut")->AsShaderResource();
	if (!m_pLut->IsValid())
		Logger::LogError(L"PostGrayscale::LoadEffectVariables() => failed to get shader variable gLut ");
}

void PostLUT::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	UNREFERENCED_PARAMETER(pRendertarget);
	//TODO: Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
	m_pLut->SetResource(m_pLutTexture->GetShaderResourceView());
}