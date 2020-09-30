#include "stdafx.h"

#include "UberScene.h"
#include "GameObject.h"
#include "Components.h"
#include "ContentManager.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "../../Materials/ColorMaterial.h"
#include "../../Materials/DiffuseMaterial.h"
#include "../../Materials/UberMaterial.h"

UberScene::UberScene() :
	GameScene(L"UberScene")
{
}

void UberScene::Initialize()
{
	const auto gameContext = GetGameContext();
	GetPhysxProxy()->EnablePhysxDebugRendering(true);

	{
		auto pTeaPot = new GameObject();
		pTeaPot->GetTransform()->Translate(-30.f, 0.f, 0.f);
		
		auto pModelComponent = new ModelComponent(L"Resources/Meshes/Teapot.ovm");
		pModelComponent ? pTeaPot->AddComponent(pModelComponent) : Logger::LogError(L"Failed to load teapot model");

		AddChild(pTeaPot);

		auto pUber = new UberMaterial();
		gameContext.pMaterialManager->AddMaterial(pUber, 69);

		// Diffuse
		pUber->SetDiffuseTexture(L"Resources/Textures/Chair_Dark.dds");
		pUber->EnableDiffuseTexture(true);

		// Phong
		pUber->EnableSpecularPhong(true);

		// Enviroment mapping
		pUber->SetEnvironmentCube(L"Resources/Textures/Sunol_Cubemap.dds");
		pUber->EnableEnvironmentMapping(true);
		pUber->SetReflectionStrength(.5f);

		pModelComponent->SetMaterial(69);
	}

	{
		auto pTeaPot = new GameObject();
		auto pModelComponent = new ModelComponent(L"Resources/Meshes/Teapot.ovm");
		pModelComponent ? pTeaPot->AddComponent(pModelComponent) : Logger::LogError(L"Failed to load teapot model");

		AddChild(pTeaPot);

		auto pUber = new UberMaterial();
		gameContext.pMaterialManager->AddMaterial(pUber, 420);

		// Diffuse
		pUber->SetDiffuseTexture(L"Resources/Textures/Chair_Dark.dds");
		pUber->EnableDiffuseTexture(true);

		// Blinn
		pUber->EnableSpecularBlinn(true);

		// Enviroment mapping
		pUber->SetEnvironmentCube(L"Resources/Textures/Sunol_Cubemap.dds");
		pUber->EnableEnvironmentMapping(true);
		pUber->SetReflectionStrength(.5f);

		// Normal mapping
		pUber->SetNormalMapTexture(L"Resources/Textures/CobbleStone_NormalMap.dds");
		pUber->EnableNormalMapping(true);

		pModelComponent->SetMaterial(420);
		pTeaPot->GetTransform()->Translate(30.f, 0.f, 0.f);
	}

	{
		auto pTeaPot = new GameObject();
		auto pModelComponent = new ModelComponent(L"Resources/Meshes/Teapot.ovm");
		pModelComponent ? pTeaPot->AddComponent(pModelComponent) : Logger::LogError(L"Failed to load teapot model");

		AddChild(pTeaPot);

		auto pUber = new UberMaterial(L"WithAlphaBlending");
		gameContext.pMaterialManager->AddMaterial(pUber, 6969);

		// Blinn
		pUber->EnableSpecularBlinn(true);

		// Opacity
		pUber->SetOpacity(0.5f);

		pModelComponent->SetMaterial(6969);
	}
}

void UberScene::Update()
{}

void UberScene::Draw()
{}
