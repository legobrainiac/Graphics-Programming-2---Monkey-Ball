#include "stdafx.h"
#include "Banana.h"
#include "Components.h"
#include "PhysxManager.h"
#include "SoundManager.h"
#include "GameScene.h"

#include "../../Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"

#include "MonkeyBallChar.h"

FMOD::Sound* Banana::m_pChime = nullptr;
int Banana::m_BananaCounter = 0;

Banana::~Banana()
{
	--m_BananaCounter;

	if (m_BananaCounter == 0)
	{
		m_pChime->release();
		m_pChime = nullptr;
	}
}

void Banana::Initialize([[maybe_unused]] const GameContext& gameContext)
{
	m_pMaterial = static_cast<SkinnedDiffuseMaterial_Shadow*>(gameContext.pMaterialManager->GetMaterial(128));
	if (!m_pMaterial)
	{
		m_pMaterial = new SkinnedDiffuseMaterial_Shadow();
		m_pMaterial->SetDiffuseTexture(L"Resources/MonkeyBall/Texture/Banana_BaseColor.png");
		m_pMaterial->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
		gameContext.pMaterialManager->AddMaterial(m_pMaterial, 128);
	}

	m_pBanana = new GameObject();
	const auto bananaModel = new ModelComponent(L"Resources/MonkeyBall/Meshes/Banana.ovm");
	bananaModel->SetMaterial(128);
	m_pBanana->AddComponent(bananaModel);

	m_pBanana->GetTransform()->Translate(0.f, 10.f, 0.f);
	m_pBanana->GetTransform()->Scale(0.1f, 0.1f, .1f);

	AddChild(m_pBanana);

	const auto pFmod = SoundManager::GetInstance()->GetSystem();
	if (!m_pChime)
		pFmod->createSound("Resources/MonkeyBall/Sound/pickup.wav", FMOD_DEFAULT, nullptr, &m_pChime);

	// Setup trigger capsule
	auto pTriggerBehaviour = [this]([[maybe_unused]] GameObject* pTrigger, [[maybe_unused]] GameObject* pOther, [[maybe_unused]] GameObject::TriggerAction action)
	{
		auto monkey = static_cast<MonkeyBallChar*>(pOther);

		if (monkey && m_Timer > 1.f && !m_HasBeenMonkeyed)
		{
			const auto pFmod = SoundManager::GetInstance()->GetSystem();

			// Barf
			m_HasBeenMonkeyed = true;
			monkey->AddScore(5);

			FMOD::Channel* pChannel = nullptr;
			pFmod->playSound(m_pChime, nullptr, false, &pChannel);
			pChannel->setVolume(0.5f);
			pChannel->setPitch(randF(0.5f, 2.f));
		}
	};

	auto pPhysX = PhysxManager::GetInstance()->GetPhysics();

	m_pTrigger = new GameObject();
	m_pTrigger->GetTransform()->Translate(0, 1000.f, 0.f);
	auto pTriggerRigidBody = new RigidBodyComponent();
	pTriggerRigidBody->SetKinematic(true);
	m_pTrigger->AddComponent(pTriggerRigidBody);

	auto pPhysicsMaterial = pPhysX->createMaterial(.0f, .0f, 1.f);
	std::shared_ptr<physx::PxGeometry> pTriggerGeometry(new physx::PxSphereGeometry(10.f));
	auto pCollider = new ColliderComponent(pTriggerGeometry, *pPhysicsMaterial, physx::PxTransform::createIdentity());
	m_pTrigger->AddComponent(pCollider);
	pCollider->EnableTrigger(true);
	m_pTrigger->SetOnTriggerCallBack(pTriggerBehaviour);

	AddChild(m_pTrigger);
}

void Banana::PostInitialize([[maybe_unused]] const GameContext& gameContext)
{
	GetTransform()->Translate(m_StartPosition);
}

void Banana::Update([[maybe_unused]] const GameContext& gameContext)
{
	const auto dt = gameContext.pGameTime->GetElapsed();

	if (m_HasBeenMonkeyed)
	{
		m_HasBeenMonkeyedTimer += dt;
		float escale = (1 - m_HasBeenMonkeyedTimer * 8.f) * 0.1f;
		m_pBanana->GetTransform()->Scale(escale, escale, escale);

		if (m_HasBeenMonkeyedTimer > 0.125f)
			GetScene()->MarkDestroy(this);

		return;
	}

	m_Timer += dt;
	
	// Rotation
	GetTransform()->Rotate(90.f, m_Timer * 90.f, 0.f);

	// Hover
	m_pBanana->GetTransform()->Translate(0.f, 0.f, m_GroundOffset + sinf(m_Timer * m_HoverSpeed) * m_HoverStrenght);
	m_pTrigger->GetTransform()->Translate(m_pBanana->GetTransform()->GetWorldPosition());
}

void Banana::Draw([[maybe_unused]] const GameContext& gameContext)
{
}
