#include "stdafx.h"
#include "MBPrefabs.h"

#include "Banana.h"
#include "GameScene.h"
#include "EndPortal.h"
#include "GameObject.h"
#include "PhysxManager.h"
#include "ContentManager.h"
#include "ModelComponent.h"
#include "RotatorComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "../../Materials/Shadow/DiffuseMaterial_Shadow.h"

#include <fstream>
#include <string>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

GameObject* MBPrefabs::SpinningObjectPrefab(uint32_t materialID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotatingSpeed)
{
	auto pPhysX = PhysxManager::GetInstance()->GetPhysics();

	auto pBouncyMaterial = pPhysX->createMaterial(1.0f, 1.0f, 0.5f);
	auto pSpinningObject = new GameObject();
	auto pModelComponent = new ModelComponent(L"Resources/MonkeyBall/Meshes/SpinningObstacle.ovm");
	pSpinningObject->AddComponent(pModelComponent);
	pModelComponent->SetMaterial(materialID);

	auto pRigidBody = new RigidBodyComponent();
	pRigidBody->SetKinematic(true);
	pSpinningObject->AddComponent(pRigidBody);
	pSpinningObject->AddComponent(new RotatorComponent(rotatingSpeed));

	auto pCMesh = ContentManager::Load<physx::PxTriangleMesh>(L"Resources/MonkeyBall/Meshes/SpinningObstacle.ovpt");
	std::shared_ptr<physx::PxGeometry> pGeometryy(new physx::PxTriangleMeshGeometry(pCMesh));

	auto pCollider = new ColliderComponent(pGeometryy, *pBouncyMaterial);
	pSpinningObject->AddComponent(pCollider);
	pSpinningObject->GetTransform()->Translate(position);

	return pSpinningObject;
}

GameObject* MBPrefabs::SkyDomePrefab(uint32_t materialID)
{
	auto skyDome = new GameObject();
	auto pSkyModel = new ModelComponent(L"Resources/MonkeyBall/Meshes/SkyDome.ovm");
	skyDome->AddComponent(pSkyModel);
	pSkyModel->SetMaterial(materialID);
	return skyDome;
}

std::vector<GameObject*> MBPrefabs::LevelPrefab(GameScene*pGameScene, const std::wstring& name, float& levelTimeOut)
{
	std::vector<GameObject*> scene{};

	const auto gameContext = pGameScene->GetGameContext();

	//////////////////////////////////////////////////////////////////////////
	auto shadowMat = static_cast<DiffuseMaterial_Shadow*>(gameContext.pMaterialManager->GetMaterial(128));
	if (!shadowMat)
	{
		shadowMat = new DiffuseMaterial_Shadow(true);
		shadowMat->SetDiffuseTexture(L"Resources/MonkeyBall/Texture/WaterYouDoing.jpg");
		gameContext.pMaterialManager->AddMaterial(shadowMat, 123);
	}

	//////////////////////////////////////////////////////////////////////////
	std::ifstream str(L"Resources/MonkeyBall/Worlds/" + name + L"/level.json");

	std::string jsonIn{};
	std::string temp{};

	while (std::getline(str, temp))
		jsonIn += temp;

	auto j = json::parse(jsonIn);
	
	levelTimeOut = 					j["play-time"];
	uint32_t bananas =				j["bananas"].size();
	uint32_t rotatingPlatforms =	j["rotating-platforms"].size();
	uint32_t endPortals =			j["end-portals"].size();

	//////////////////////////////////////////////////////////////////////////
	// Parse bananas
	for (uint32_t i = 0U; i < bananas; ++i)
	{
		float x = j["bananas"][i][0];
		float y = j["bananas"][i][1];
		float z = j["bananas"][i][2];

		auto banana = new Banana({ x, y, z });
		scene.push_back(banana);
		pGameScene->AddChild(banana);
	}

	//////////////////////////////////////////////////////////////////////////
	// Parse rotating platforms
	for (uint32_t i = 0U; i < rotatingPlatforms; ++i)
	{
		float x = j["rotating-platforms"][i]["position"][0];
		float y = j["rotating-platforms"][i]["position"][1];
		float z = j["rotating-platforms"][i]["position"][2];

		float xr = j["rotating-platforms"][i]["rotation-speed"][0];
		float yr = j["rotating-platforms"][i]["rotation-speed"][1];
		float zr = j["rotating-platforms"][i]["rotation-speed"][2];

		auto spinningObject = MBPrefabs::SpinningObjectPrefab(123, { x, y, z }, { xr, yr, zr });
		scene.push_back(spinningObject);
		pGameScene->AddChild(spinningObject);
	}

	//////////////////////////////////////////////////////////////////////////
	// Parse end portals
	for (uint32_t i = 0U; i < endPortals; ++i)
	{
		float x = j["end-portals"][i]["position"][0];
		float y = j["end-portals"][i]["position"][1];
		float z = j["end-portals"][i]["position"][2];

		float rz = j["end-portals"][i]["rotation"];

		auto portal = new EndPortal({ x, y, z }, rz);
		scene.push_back(portal);
		pGameScene->AddChild(portal);
	}

	auto pPhysX = PhysxManager::GetInstance()->GetPhysics();
	auto pBouncyMaterial = pPhysX->createMaterial(1.0f, 1.0f, 0.5f);

	auto level = new GameObject();
	scene.push_back(level);
	auto pModelComponent = new ModelComponent(L"Resources/MonkeyBall/Worlds/" + name + L"/level.ovm");
	pModelComponent->SetMaterial(123);
	level->AddComponent(pModelComponent);

	auto pRigidBody = new RigidBodyComponent();
	pRigidBody->SetKinematic(true);
	level->AddComponent(pRigidBody);

	auto pCMesh = ContentManager::Load<physx::PxTriangleMesh>(L"Resources/MonkeyBall/Worlds/" + name + L"/level.ovpt");
	std::shared_ptr<physx::PxGeometry> pGeometryy(new physx::PxTriangleMeshGeometry(pCMesh));

	auto pCollider = new ColliderComponent(pGeometryy, *pBouncyMaterial);
	level->AddComponent(pCollider);
	pGameScene->AddChild(level);

	return scene;
}
