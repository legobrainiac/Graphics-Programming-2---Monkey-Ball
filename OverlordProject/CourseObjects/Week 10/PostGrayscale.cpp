#include "stdafx.h"
#include "PostGrayscale.h"
#include "RenderTarget.h"

PostGrayscale::PostGrayscale()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Grayscale.fx", 1),
	m_pTextureMapVariabele(nullptr)
{
}

void PostGrayscale::LoadEffectVariables()
{
	//TODO: Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	//Check if valid!
	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariabele->IsValid())
		Logger::LogError(L"PostGrayscale::LoadEffectVariables() => failed to get shader variable gTexture ");
}

void PostGrayscale::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	UNREFERENCED_PARAMETER(pRendertarget);

	//TODO: Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
}