#include "stdafx.h"
#include "BoneObject.h"
#include "TransformComponent.h"
#include "SoftwareSkinningScene_1.h"
#include "../../Materials/ColorMaterial.h"

SoftwareSkinningScene_1::SoftwareSkinningScene_1()
	: GameScene(L"SoftwareSkinningScene_1")
	, m_pBone0(nullptr)
	, m_pBone1(nullptr)
	, m_BoneRotation(0.f)
	, m_RotationSign(1)
{
}

void SoftwareSkinningScene_1::Initialize()
{
	const auto gameContext = GetGameContext();
	auto pColourMaterial = new ColorMaterial();
	gameContext.pMaterialManager->AddMaterial(pColourMaterial, 0);

	// Create bone structure
	GameObject* pRoot = new GameObject();
	m_pBone0 = new BoneObject(0, 0, 15.f);
	m_pBone1 = new BoneObject(1, 0, 15.f);

	AddChild(pRoot);
	pRoot->AddChild(m_pBone0);
	m_pBone0->AddBone(m_pBone1);
}

void SoftwareSkinningScene_1::Update()
{
	const auto gameContext = GetGameContext();
	m_BoneRotation += m_RotationSign * 45.f * gameContext.pGameTime->GetElapsed();

	if (m_BoneRotation < -45.f || m_BoneRotation > 45.f)
		m_RotationSign *= -1;

	m_pBone0->GetTransform()->Rotate(0.f, 0.f, m_BoneRotation);
	m_pBone1->GetTransform()->Rotate(0.f, 0.f, -m_BoneRotation * 2.f);
}

void SoftwareSkinningScene_1::Draw()
{}
