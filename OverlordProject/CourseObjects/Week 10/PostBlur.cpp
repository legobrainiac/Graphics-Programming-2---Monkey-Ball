#include "stdafx.h"
#include "PostBlur.h"
#include "RenderTarget.h"

PostBlur::PostBlur()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Blur.fx", 2),
	m_pTextureMapVariabele(nullptr)
{
}

void PostBlur::LoadEffectVariables()
{
	//TODO: Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	//Check if valid!
	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariabele->IsValid())
		Logger::LogError(L"PostGrayscale::LoadEffectVariables() => failed to get shader variable gTexture ");
}

void PostBlur::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
}