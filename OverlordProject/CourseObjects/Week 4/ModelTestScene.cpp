#include "stdafx.h"

#include "ModelTestScene.h"
#include "GameObject.h"
#include "Components.h"
#include "ContentManager.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "../../Materials/ColorMaterial.h"
#include "../../Materials/DiffuseMaterial.h"
#include "../../Materials/UberMaterial.h"

ModelTestScene::ModelTestScene() :
	GameScene(L"ModelTestScene"),
	m_pChair(nullptr)
{}

void ModelTestScene::Initialize()
{
	const auto gameContext = GetGameContext();
	GetPhysxProxy()->EnablePhysxDebugRendering(true);

	//GROUND PLANE
	//************
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto pBouncyMaterial = physX->createMaterial(0, 0, 1);
	auto pGround = new GameObject();
	pGround->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	pGround->AddComponent(new ColliderComponent(geom, *pBouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGround);

	//CHAIR OBJECT
	//************
	m_pChair = new GameObject();
	auto pModelComponent = new ModelComponent(L"Resources/Meshes/Chair.ovm");
	pModelComponent ? m_pChair->AddComponent(pModelComponent) : Logger::LogError(L"Failed to load chair model");

	//1. Attach a modelcomponent (chair.ovm)
	//2. Create a ColorMaterial and add it to the material manager
	//3. Assign the material to the previous created modelcomponent
	// Build and Run

	//4. Create a DiffuseMaterial (using PosNormTex3D.fx)
	//		Make sure you are able to set a texture (DiffuseMaterial::SetDiffuseTexture(const wstring& assetFile))
	//		Load the correct shadervariable and set it during the material variable update
	//5. Assign the material to the modelcomponent
	// Build and Run
		
	auto pDiffuseMaterial = new DiffuseMaterial();
	pDiffuseMaterial->SetDiffuseTexture(L"Resources/Textures/Chair_Dark.dds");
	gameContext.pMaterialManager->AddMaterial(pDiffuseMaterial, 1);
	pModelComponent->SetMaterial(1);
	//6. Attach a rigidbody component (pure-dynamic)
	//7. Attach a collider component (Use a PxConvexMeshGeometry [chair.ovpc])
	// Build and Run

	auto pRigidBody = new RigidBodyComponent();
	m_pChair->AddComponent(pRigidBody);

    auto pCMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Chair.ovpc");
	std::shared_ptr<physx::PxGeometry> pGeometry(new physx::PxConvexMeshGeometry(pCMesh));

	auto pCollider = new ColliderComponent(pGeometry, *pBouncyMaterial);
	m_pChair->AddComponent(pCollider);
	m_pChair->GetTransform()->Translate(0.f, 0.1f, 0.f);
	
	AddChild(m_pChair);
}

void ModelTestScene::Update()
{}

void ModelTestScene::Draw()
{}
