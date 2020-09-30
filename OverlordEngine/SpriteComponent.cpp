#include "stdafx.h"

#include "SpriteComponent.h"
 #include <utility>

#include "GameObject.h"
#include "TextureData.h"
#include "ContentManager.h"
#include "SpriteRenderer.h"
#include "TransformComponent.h"

SpriteComponent::SpriteComponent(std::wstring spriteAsset, DirectX::XMFLOAT2 pivot, DirectX::XMFLOAT4 color):
	m_pTexture(nullptr),
	m_SpriteAsset(std::move(spriteAsset)),
	m_Pivot(pivot),
	m_Color(color)
{
}

void SpriteComponent::Initialize(const GameContext& )
{
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::SetTexture(const std::wstring& spriteAsset)
{
	m_SpriteAsset = spriteAsset;
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::Update(const GameContext& )
{
}

void SpriteComponent::Draw(const GameContext& )
{
	if (!m_pTexture)
		return;

	auto pSpriteRender = SpriteRenderer::GetInstance();
	auto pTransform = GetTransform();
	
	DirectX::XMFLOAT2 f2pos = { pTransform->GetPosition().x, pTransform->GetPosition().y };
	DirectX::XMFLOAT2 f2scale = { pTransform->GetScale().x, pTransform->GetScale().y };
	DirectX::XMFLOAT3 rot = QuaternionToEuler(pTransform->GetRotation());

	pSpriteRender->Draw(m_pTexture, f2pos, m_Color, m_Pivot, f2scale, rot.z);
}
