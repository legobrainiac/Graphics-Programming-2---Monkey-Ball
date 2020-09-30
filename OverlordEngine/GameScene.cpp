#include "stdafx.h"
#include "GameScene.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "OverlordGame.h"
#include "Prefabs.h"
#include "Components.h"
#include "DebugRenderer.h"
#include "RenderTarget.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "PhysxProxy.h"
#include "SoundManager.h"
#include "PostProcessingMaterial.h"
#include <algorithm>

GameScene::GameScene(std::wstring sceneName):
	m_pChildren(std::vector<GameObject*>()),
	m_GameContext(GameContext()),
	m_IsInitialized(false),
	m_SceneName(std::move(sceneName)),
	m_pDefaultCamera(nullptr),
	m_pActiveCamera(nullptr),
	m_pPhysxProxy(nullptr)
{
}

GameScene::~GameScene()
{
	SafeDelete(m_GameContext.pGameTime);
	SafeDelete(m_GameContext.pInput);
	SafeDelete(m_GameContext.pMaterialManager);
	SafeDelete(m_GameContext.pShadowMapper);

	for (auto pChild : m_pChildren)
	{
		SafeDelete(pChild);
	}
	SafeDelete(m_pPhysxProxy);

	for (auto ppm : m_PostProcessingEffects)
		SafeDelete(ppm);
}

void GameScene::AddChild(GameObject* obj)
{
#if _DEBUG
	if (obj->m_pParentScene)
	{
		if (obj->m_pParentScene == this)
			Logger::LogWarning(L"GameScene::AddChild > GameObject is already attached to this GameScene");
		else
			Logger::LogWarning(
				L"GameScene::AddChild > GameObject is already attached to another GameScene. Detach it from it's current scene before attaching it to another one.");

		return;
	}

	if (obj->m_pParentObject)
	{
		Logger::LogWarning(
			L"GameScene::AddChild > GameObject is currently attached to a GameObject. Detach it from it's current parent before attaching it to another one.");
		return;
	}
#endif

	obj->m_pParentScene = this;
	obj->RootInitialize(m_GameContext);
	m_pChildren.push_back(obj);
}

void GameScene::RemoveChild(GameObject* obj, bool deleteObject)
{
	const auto it = find(m_pChildren.begin(), m_pChildren.end(), obj);

	if (it == m_pChildren.end())
		return;

	m_pChildren.erase(it);
	if (deleteObject)
	{
		delete obj;
		obj = nullptr;
	}
	else
		obj->m_pParentScene = nullptr;
}

void GameScene::MarkDestroy(GameObject* obj)
{
	m_pDestroyBuffer.push_back(obj);
}

void GameScene::RootInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	if (m_IsInitialized)
		return;

	//Create DefaultCamera
	auto freeCam = new FreeCamera();
	freeCam->SetRotation(30, 0);
	freeCam->GetTransform()->Translate(0, 50, -80);
	AddChild(freeCam);
	m_pDefaultCamera = freeCam->GetComponent<CameraComponent>();
	m_pActiveCamera = m_pDefaultCamera;
	m_GameContext.pCamera = m_pDefaultCamera;

	//Create GameContext
	m_GameContext.pGameTime = new GameTime();
	m_GameContext.pGameTime->Reset();
	m_GameContext.pGameTime->Stop();

	m_GameContext.pInput = new InputManager();
	InputManager::Initialize();

	m_GameContext.pMaterialManager = new MaterialManager();
	m_GameContext.pShadowMapper = new ShadowMapRenderer();

	m_GameContext.pDevice = pDevice;
	m_GameContext.pDeviceContext = pDeviceContext;

	//Initialize ShadowMapper
	m_GameContext.pShadowMapper->Initialize(m_GameContext);

	// Initialize Physx
	m_pPhysxProxy = new PhysxProxy();
	m_pPhysxProxy->Initialize(this);

	//User-Scene Initialize
	Initialize();

	//Root-Scene Initialize
	for (auto pChild : m_pChildren)
	{
		pChild->RootInitialize(m_GameContext);
	}

	for (auto ppm : m_PostProcessingEffects)
		ppm->Initialize(m_GameContext);

	m_IsInitialized = true;
}

void GameScene::RootUpdate()
{
	m_GameContext.pGameTime->Update();
	m_GameContext.pInput->Update();
	m_GameContext.pCamera = m_pActiveCamera;

	SoundManager::GetInstance()->GetSystem()->update();

	//User-Scene Update
	Update();

	//Root-Scene Update
	for (auto pChild : m_pChildren)
	{
		pChild->RootUpdate(m_GameContext);
	}

	std::sort(
		m_PostProcessingEffects.begin(),
		m_PostProcessingEffects.end(),
		[](PostProcessingMaterial* l, PostProcessingMaterial* r) 
		{
			return l->GetRenderIndex() < r->GetRenderIndex();
		}
	);

	m_pPhysxProxy->Update(m_GameContext);

	for (auto obj : m_pDestroyBuffer)
		RemoveChild(obj, true);

	m_pDestroyBuffer.clear();
}

void GameScene::RootDraw()
{
	if (m_OtherScene)
	{
		m_OtherScene->RootDraw();
		Draw();
		return;
	}

	//TODO: object-Scene SHADOW_PASS - start by setting the correct render target, render all to shadow map and end by reset default render target
	m_GameContext.pShadowMapper->Begin(m_GameContext);

	for (auto pChild : m_pChildren)
		pChild->RootDrawShadowMap(m_GameContext);

	m_GameContext.pShadowMapper->End(m_GameContext);

	//User-Scene Draw
	Draw();

	//Object-Scene Draw
	for (auto pChild : m_pChildren)
	{
		pChild->RootDraw(m_GameContext);
	}

	//Object-Scene Post-Draw
	for (auto pChild : m_pChildren)
	{
		pChild->RootPostDraw(m_GameContext);
	}

	//Draw PhysX
	m_pPhysxProxy->Draw(m_GameContext);

	//Draw Debug Stuff
	DebugRenderer::Draw(m_GameContext);
	SpriteRenderer::GetInstance()->Draw(m_GameContext);
	TextRenderer::GetInstance()->Draw(m_GameContext);

	if (m_PostProcessingEffects.empty())
		return;

	const auto pGame = SceneManager::GetInstance()->GetGame();
	auto currentRenderTarget = pGame->GetRenderTarget();
	auto previousRenderTarget = currentRenderTarget;

	for (auto PPE : m_PostProcessingEffects)
	{
		auto pRt = PPE->GetRenderTarget();
		pGame->SetRenderTarget(pRt);
		PPE->Draw(m_GameContext, previousRenderTarget);
		previousRenderTarget = pRt;
	}
	
	pGame->SetRenderTarget(currentRenderTarget);
	SpriteRenderer::GetInstance()->DrawImmediate(m_GameContext, previousRenderTarget->GetShaderResourceView(), {});
}

void GameScene::RootSceneActivated()
{
	//Start Timer
	m_GameContext.pGameTime->Start();
	SceneActivated();
}

void GameScene::RootSceneDeactivated()
{
	//Stop Timer
	m_GameContext.pGameTime->Stop();
	SceneDeactivated();
}

void GameScene::RootWindowStateChanged(int state, bool active) const
{
	//TIMER
	if (state == 0)
	{
		if (active)m_GameContext.pGameTime->Start();
		else m_GameContext.pGameTime->Stop();
	}
}

void GameScene::SetActiveCamera(CameraComponent* pCameraComponent)
{
	if (m_pActiveCamera != nullptr)
		m_pActiveCamera->m_IsActive = false;

	m_pActiveCamera = (pCameraComponent) ? pCameraComponent : m_pDefaultCamera;
	m_pActiveCamera->m_IsActive = true;
}

void GameScene::AddPostProcessingEffect(PostProcessingMaterial* pEffect)
{
	const auto it = std::find(m_PostProcessingEffects.cbegin(), m_PostProcessingEffects.cend(), pEffect);
	
	if (it == m_PostProcessingEffects.cend())
		m_PostProcessingEffects.push_back(pEffect);
	else
		Logger::LogError(L"Effect already in post processing chain ting");
	
	pEffect->Initialize(GetGameContext());
}

void GameScene::RemovePostProcessingEffect(PostProcessingMaterial* pEffect)
{
	const auto it = std::find(m_PostProcessingEffects.cbegin(), m_PostProcessingEffects.cend(), pEffect);
	delete (*it);
	m_PostProcessingEffects.erase(it);
}