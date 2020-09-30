#pragma once
#include "Material.h"

class TextureData;

class DiffuseMaterial final 
	: public Material
{
public:
	DiffuseMaterial(std::wstring effectFile = L"./Resources/Effects/PosNormTex3D.fx");
	~DiffuseMaterial();

	void SetDiffuseTexture(const std::wstring& assetFile);

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);

	TextureData* m_pDiffuseTexture;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	DiffuseMaterial(const DiffuseMaterial& obj);
	DiffuseMaterial& operator=(const DiffuseMaterial& obj);
};

