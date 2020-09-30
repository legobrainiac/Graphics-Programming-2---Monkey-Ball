#include "stdafx.h"
#include "EndPortal.h"

#include "GameObject.h"
#include "PhysxManager.h"
#include "ModelComponent.h"
#include "ContentManager.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "MonkeyBallMainScene.h"
#include "../../Materials/DiffuseMaterial.h"
#include "../../Materials/Shadow/DiffuseMaterial_Shadow.h"

EndPortal::EndPortal(DirectX::XMFLOAT3 pos, float rot)
	: m_Pos(pos)
	, m_Rot(rot)
{
}

void EndPortal::Initialize([[maybe_unused]] const GameContext& gameContext)
{
	auto pMaterial = static_cast<DiffuseMaterial*>(gameContext.pMaterialManager->GetMaterial(512));
	if (!pMaterial)
	{
		pMaterial = new DiffuseMaterial();
		pMaterial->SetDiffuseTexture(L"Resources/MonkeyBall/Texture/ice.jpg");
		gameContext.pMaterialManager->AddMaterial(pMaterial, 512);
	}

	auto pMaterial2 = static_cast<DiffuseMaterial_Shadow*>(gameContext.pMaterialManager->GetMaterial(512));
	if (!pMaterial)
	{
		pMaterial2 = new DiffuseMaterial_Shadow();
		pMaterial2->SetDiffuseTexture(L"Resources/MonkeyBall/Texture/WaterYouDoing.jpg");
		gameContext.pMaterialManager->AddMaterial(pMaterial, 123);
	}


	m_pModels = new GameObject();

	//////////////////////////////////////////////////////////////////////////
	// Pylon model and collider
	auto pPhysX = PhysxManager::GetInstance()->GetPhysics();
	auto pBouncyMaterial = pPhysX->createMaterial(1.0f, 1.0f, 0.5f);

	{
		const auto model = new ModelComponent(L"Resources/MonkeyBall/Meshes/EndPortal.ovm");
		model->SetMaterial(512);
		m_pModels->AddComponent(model);

		auto pRigidBody = new RigidBodyComponent();
		pRigidBody->SetKinematic(true);
		m_pModels->AddComponent(pRigidBody);

		auto pCMesh = ContentManager::Load<physx::PxTriangleMesh>(L"Resources/MonkeyBall/Meshes/EndPortal.ovpt");
		std::shared_ptr<physx::PxGeometry> pGeometryy(new physx::PxTriangleMeshGeometry(pCMesh));

		auto pCollider = new ColliderComponent(pGeometryy, *pBouncyMaterial);
		m_pModels->AddComponent(pCollider);
	}

	{
		const auto model = new ModelComponent(L"Resources/MonkeyBall/Meshes/EndPortalInside.ovm");
		model->SetMaterial(123);
		m_pModels->AddComponent(model);
	}
	
	//////////////////////////////////////////////////////////////////////////
	// Trigger
	{
		// Setup trigger capsule
		auto pTriggerBehaviour = [this]([[maybe_unused]] GameObject* pTrigger, [[maybe_unused]] GameObject* pOther, [[maybe_unused]] GameObject::TriggerAction action)
		{
			if (action == GameObject::TriggerAction::ENTER && m_Timer > 1.f)
				static_cast<MonkeyBallMainScene*>(pOther->GetScene())->QueueNextLevel();
		};

		m_pTrigger = new GameObject();
		auto pTriggerRigidBody = new RigidBodyComponent();
		pTriggerRigidBody->SetKinematic(true);
		m_pTrigger->AddComponent(pTriggerRigidBody);

		auto pPhysicsMaterial = pPhysX->createMaterial(.0f, .0f, 1.f);
		std::shared_ptr<physx::PxGeometry> pTriggerGeometry(new physx::PxSphereGeometry(50.f));
		auto pCollider = new ColliderComponent(pTriggerGeometry, *pPhysicsMaterial, physx::PxTransform::createIdentity());
		m_pTrigger->AddComponent(pCollider);
		pCollider->EnableTrigger(true);
		m_pTrigger->SetOnTriggerCallBack(pTriggerBehaviour);

		AddChild(m_pTrigger);
	}

	AddChild(m_pModels);
}

void EndPortal::PostInitialize([[maybe_unused]] const GameContext& gameContext)
{
	m_pModels->GetTransform()->Translate(m_Pos);
	m_pModels->GetTransform()->Rotate(0.f, m_Rot, 0.f);
}

void EndPortal::Update([[maybe_unused]] const GameContext& gameContext)
{
	m_Timer += gameContext.pGameTime->GetElapsed();
	m_pTrigger->GetTransform()->Translate(m_Pos);
}

void EndPortal::Draw([[maybe_unused]] const GameContext& gameContext)
{
}
