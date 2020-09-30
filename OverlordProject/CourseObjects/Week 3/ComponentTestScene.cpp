//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ComponentTestScene.h"
#include "Prefabs.h"
#include "Components.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"

ComponentTestScene::ComponentTestScene(void) 
	: GameScene(L"ComponentTestScene")
{
}

void ComponentTestScene::Initialize()
{
	Logger::LogInfo(L"Big lmao!");

	GetPhysxProxy()->EnablePhysxDebugRendering(true);

	auto pPhysX = PhysxManager::GetInstance()->GetPhysics();
	auto pBouncyMaterial = pPhysX->createMaterial(.5f, .5f, 1.f);

	// Ground
	auto pGo = new GameObject();
	pGo->AddComponent(new RigidBodyComponent(true));
	
	// Plane geometry
	std::shared_ptr<physx::PxGeometry> pGeometry(new physx::PxPlaneGeometry());
	pGo->AddComponent(new ColliderComponent(pGeometry, *pBouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0.f, 0.f, 1.f)))));
	
	AddChild(pGo);

	// Sphere
	auto pSphere = new SpherePrefab();
	pSphere->GetTransform()->Translate(0.f, 30.f, 0.f);
	
	// Rigidbody component
	auto pRigidBodyComponent = new RigidBodyComponent();
	pRigidBodyComponent->SetCollisionGroup(CollisionGroupFlag::Group0);
	pRigidBodyComponent->SetCollisionIgnoreGroups((CollisionGroupFlag)((UINT)CollisionGroupFlag::Group1 | (UINT)CollisionGroupFlag::Group2));
	pSphere->AddComponent(pRigidBodyComponent);
	
	// Geometry
	std::shared_ptr<physx::PxGeometry> pSphereGeometry(new physx::PxSphereGeometry(1.f));
	pSphere->AddComponent(new ColliderComponent(pSphereGeometry, *pBouncyMaterial, physx::PxTransform::createIdentity()));

	AddChild(pSphere);

	// Sphere 2
	auto pSphere2 = new SpherePrefab();
	pSphere2->GetTransform()->Translate(0.f, 20.f, 0.f);

	auto pRigidBodyComponent2 = new RigidBodyComponent();
	pRigidBodyComponent2->SetCollisionGroup(CollisionGroupFlag::Group1);
	pSphere2->AddComponent(pRigidBodyComponent2);

	// Geometry
	std::shared_ptr<physx::PxGeometry> pSphereGeometry2(new physx::PxSphereGeometry(1.f));
	pSphere2->AddComponent(new ColliderComponent(pSphereGeometry2, *pBouncyMaterial, physx::PxTransform::createIdentity()));

	AddChild(pSphere2);

	// Sphere 3
	auto pSphere3 = new SpherePrefab();
	pSphere3->GetTransform()->Translate(0.f, 10.f, 0.f);

	auto pRigidBodyComponent3 = new RigidBodyComponent();
	pRigidBodyComponent3->SetCollisionGroup(CollisionGroupFlag::Group2);
	pSphere3->AddComponent(pRigidBodyComponent3);

	// Geometry
	std::shared_ptr<physx::PxGeometry> pSphereGeometry3(new physx::PxSphereGeometry(1.f));
	pSphere3->AddComponent(new ColliderComponent(pSphereGeometry3, *pBouncyMaterial, physx::PxTransform::createIdentity()));

	AddChild(pSphere3);
}

void ComponentTestScene::Update()
{
	const auto gameContext = GetGameContext();
	m_FpsInterval += gameContext.pGameTime->GetElapsed();
	if (m_FpsInterval >= 1.f)
	{
		--m_FpsInterval;
		Logger::LogFormat(LogLevel::Info, L"FPS:%i", gameContext.pGameTime->GetFPS());
	}
}

void ComponentTestScene::Draw()
{
}
