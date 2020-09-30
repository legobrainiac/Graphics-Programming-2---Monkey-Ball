
#include "stdafx.h"

#include "Prefabs.h"
#include "Components.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"
#include "CharacterScene.h"
#include "Character.h"
#include "ContentManager.h"
#include "../../Materials/ColorMaterial.h"

CharacterScene::CharacterScene(void)
	: GameScene(L"CharacterScene")
{
}

void CharacterScene::Initialize()
{
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
	AddChild(new Character());

	// Level
	const auto gameContext = GetGameContext();
	GetPhysxProxy()->EnablePhysxDebugRendering(true);

	//GROUND PLANE
	//************
	auto pGround = new GameObject();
	pGround->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	pGround->AddComponent(new ColliderComponent(geom, *pBouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGround);

	// Level
	auto level = new GameObject();
	auto pModelComponent = new ModelComponent(L"Resources/Meshes/SimpleLevel.ovm");
	pModelComponent ? level->AddComponent(pModelComponent) : Logger::LogError(L"Failed to load SimpleLevel model");

	auto pColorMaterial = new ColorMaterial();
	gameContext.pMaterialManager->AddMaterial(pColorMaterial, 123);
	pModelComponent->SetMaterial(123);

	auto pRigidBody = new RigidBodyComponent();
	pRigidBody->SetKinematic(true);
	level->AddComponent(pRigidBody);

	auto pCMesh = ContentManager::Load<physx::PxTriangleMesh>(L"Resources/Meshes/SimpleLevel.ovpt");
	std::shared_ptr<physx::PxGeometry> pGeometryy(new physx::PxTriangleMeshGeometry(pCMesh));

	auto pCollider = new ColliderComponent(pGeometryy, *pBouncyMaterial);
	level->AddComponent(pCollider);
	level->GetTransform()->Translate(0.f, 0.1f, 0.f);

	AddChild(level);
}

void CharacterScene::Update()
{
}

void CharacterScene::Draw()
{
}
