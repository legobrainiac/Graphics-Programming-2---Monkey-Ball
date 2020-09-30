
#include "stdafx.h"

#include "Banana.h"
#include "Prefabs.h"
#include "PostLUT.h"
#include "EndPortal.h"
#include "MBPrefabs.h"
#include "SpriteFont.h"
#include "Components.h"
#include "PhysxProxy.h"
#include "TextureData.h"
#include "SoundManager.h"
#include "PhysxManager.h"
#include "TextRenderer.h"
#include "SceneManager.h"
#include "DebugRenderer.h"
#include "ModelAnimator.h"
#include "MonkeyBallChar.h"
#include "ContentManager.h"
#include "SpriteRenderer.h"
#include "MonkeyBallMainScene.h"
#include "../Week 10/PostBlur.h"
#include "../../Materials/DiffuseMaterial.h"
#include "../../Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"

// TODO(tomas): make camera move a bit with movement

MonkeyBallMainScene::MonkeyBallMainScene(void)
	: GameScene(L"MonkeyBallMain")
{
}

MonkeyBallMainScene::~MonkeyBallMainScene()
{
	m_pOST->release();
	m_pWhistle->release();
}

void MonkeyBallMainScene::LoadLevel(const std::wstring& level)
{
	for (auto obj : m_CurrentSceneObjects)
		RemoveChild(obj);

	m_CurrentSceneObjects = MBPrefabs::LevelPrefab(this, level, m_CurrentLevelTime);

	if (m_pPlayer)
		m_pPlayer->GetTransform()->Translate(0.f, 20.f, 0.f);
}

void MonkeyBallMainScene::LoadSequentialLevel()
{
	if (!m_LoadNext)
		return;

	++m_WorldIndex;

	if (m_WorldIndex >= Levels::WORLDCOUNT)
		m_WorldIndex = 0U;

	LoadLevel(m_Levels[m_WorldIndex]);
	m_LoadNext = false;
	
	if (m_pPlayer)
		static_cast<MonkeyBallChar*>(m_pPlayer)->ResetPosition();
}

void MonkeyBallMainScene::ReloadCurrentLevel()
{
	LoadLevel(m_Levels[m_WorldIndex]);
}

void MonkeyBallMainScene::Reset()
{
	if (m_pPlayer)
	{
		m_pPlayer->AddScore(-m_pPlayer->GetScore());
		m_pPlayer->ResetPosition();
	}

	m_WorldIndex = 0;
	m_LoadNext = false;
	m_LoadTimer = 0.f;

	const auto pFmod = SoundManager::GetInstance()->GetSystem();

	bool isplaying = false;
	pChannel->isPlaying(&isplaying);

	if (!isplaying)
	{
		pFmod->playSound(m_pOST, nullptr, false, &pChannel);
		pChannel->setVolume(0.15f);
	}

	LoadLevel(L"World1");
}

void MonkeyBallMainScene::Initialize()
{
	GetPhysxProxy()->GetPhysxScene()->setGravity(physx::PxVec3(0.0f, -98.1f, 0.0f));
	const auto gameContext = GetGameContext();
	const auto pFmod = SoundManager::GetInstance()->GetSystem();
	
	pFmod->createSound("Resources/MonkeyBall/Sound/smb2_w3_ost.mp3", FMOD_LOOP_NORMAL, nullptr, &m_pOST);
	pFmod->createSound("Resources/MonkeyBall/Sound/whistle.wav", FMOD_DEFAULT, nullptr, &m_pWhistle);

	pFmod->playSound(m_pOST, nullptr, false, &pChannel);
	pChannel->setVolume(0.15f);

	gameContext.pShadowMapper->SetSize(300.f);
	gameContext.pShadowMapper->SetFar(200.f);
	gameContext.pShadowMapper->SetLight({ -0.740129888f * 300.f - 270, 0.597205281f * 300.f, -0.309117377f * 300.f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//////////////////////////////////////////////////////////////////////////
	// Load level from json descriptor
	LoadLevel(L"World1");

	//////////////////////////////////////////////////////////////////////////
	// Sky Dome
	auto skyDomeMat = new DiffuseMaterial();
	skyDomeMat->SetDiffuseTexture(L"Resources/MonkeyBall/Texture/SkyDome.png");
	gameContext.pMaterialManager->AddMaterial(skyDomeMat, 64);
	auto skyDome = MBPrefabs::SkyDomePrefab(64);
	AddChild(skyDome);

	m_pPost = new PostLUT();
	AddPostProcessingEffect(m_pPost);
	m_PostProcessing = true;
	gameContext.pInput->AddInputAction(InputAction(420, InputTriggerState::Released, 'K', -1, XINPUT_GAMEPAD_X));
	gameContext.pInput->AddInputAction(InputAction(69, InputTriggerState::Released, 'R', -1));
	gameContext.pInput->AddInputAction(InputAction(489, InputTriggerState::Released, 'N', -1));
	gameContext.pInput->AddInputAction(InputAction(1280, InputTriggerState::Released, VK_ESCAPE, -1, XINPUT_GAMEPAD_START));
	
  	m_pPlayer = new MonkeyBallChar(skyDome);
  	AddChild(m_pPlayer);

	//DebugRenderer::ToggleDebugRenderer();
	GetPhysxProxy()->EnablePhysxDebugRendering(false);

	//////////////////////////////////////////////////////////////////////////
	// Load sprite rendering stuff
	m_pTimingBoard = ContentManager::Load<TextureData>(L"Resources/MonkeyBall/Texture/timeBoard.png");
	m_pScore = ContentManager::Load<TextureData>(L"Resources/MonkeyBall/Texture/score.png");

	for(uint32_t i = 0U; i < 10; ++i)
		m_Numbers[i] = ContentManager::Load<TextureData>(L"Resources/MonkeyBall/Texture/Numbers/" + std::to_wstring(i) + L".png");
}

void MonkeyBallMainScene::Update()
{
	const auto gameContext = GetGameContext();
	const auto pFmod = SoundManager::GetInstance()->GetSystem();
	
	if (gameContext.pInput->IsActionTriggered(1280))
	{
		SceneManager::GetInstance()->GetGameScene(L"MonkeyBallInGameMenuScene")->SetRenderOtherScene(this);
		SceneManager::GetInstance()->SetActiveGameScene(L"MonkeyBallInGameMenuScene");
	}

	if (m_CurrentLevelTime <= 0.f)
	{
		SceneManager::GetInstance()->GetGameScene(L"MonkeyBallEnd")->SetRenderOtherScene(this);
		SceneManager::GetInstance()->SetActiveGameScene(L"MonkeyBallEnd");
	}

	if (m_LoadNext)
	{
		if (m_LoadTimer <= 0.01f)
		{
			pChannel->stop();

			FMOD::Channel* pChannelW = nullptr;
			pFmod->playSound(m_pWhistle, nullptr, false, &pChannelW);

			if (m_pPlayer)
				m_pPlayer->Yeet();
		}

		m_LoadTimer += gameContext.pGameTime->GetElapsed();

		if (m_LoadTimer > 2.5f)
		{
			if (m_WorldIndex == Levels::WORLDCOUNT - 1)
			{
				SceneManager::GetInstance()->GetGameScene(L"MonkeyBallEnd")->SetRenderOtherScene(this);
				SceneManager::GetInstance()->SetActiveGameScene(L"MonkeyBallEnd");
				m_WorldIndex = 0;
				return;
			}

			LoadSequentialLevel();

			bool isplaying = false;
			pChannel->isPlaying(&isplaying);

			if (!isplaying)
			{
				pFmod->playSound(m_pOST, nullptr, false, &pChannel);
				pChannel->setVolume(0.15f);
			}

			m_LoadTimer = 0.f;
		}
	}

	if (gameContext.pInput->IsActionTriggered(69))
		ReloadCurrentLevel();

	if (gameContext.pInput->IsActionTriggered(489))
		QueueNextLevel();

	if (gameContext.pInput->IsActionTriggered(420))
	{
		if (m_PostProcessing)
			RemovePostProcessingEffect(m_pPost);
		else
		{
			m_pPost = new PostLUT();
			AddPostProcessingEffect(m_pPost);
		}

		m_PostProcessing = !m_PostProcessing;
	}

	m_CurrentLevelTime -= gameContext.pGameTime->GetElapsed();
}

void MonkeyBallMainScene::Draw()
{
	//	____  ______  __          __     _____  ______ 
 	//	|  _ \|  ____| \ \        / /\   |  __ \|  ____|
 	//	| |_) | |__     \ \  /\  / /  \  | |__) | |__   
 	//	|  _ <|  __|     \ \/  \/ / /\ \ |  _  /|  __|  
 	//	| |_) | |____     \  /\  / ____ \| | \ \| |____ 
 	//	|____/|______|     \/  \/_/    \_\_|  \_\______|
	//
	// OF THE HARDCODENING, I'm sorry, it's all hard coded... 
	// But I'm no changing it                                                 
        
	const auto spriteRenderer = SpriteRenderer::GetInstance();
	
	const auto DrawNumbers = [this](uint32_t number, DirectX::XMFLOAT2 position, bool center = true)
	{
		const auto spriteRenderer = SpriteRenderer::GetInstance();

		constexpr auto offset = 48U;
		constexpr auto ssOffset = 48.f;
		const auto numberString = std::to_string(number);

		for(size_t i = 0; i < numberString.size(); ++i)
		{
			auto currPos = position;

			if (center)
				currPos.x -= numberString.size() * 16.f;

			spriteRenderer->Draw(m_Numbers[numberString[i] - offset], currPos, { 1.f, 1.f, 1.f, 1.f }, { 0.5f, 0.5f }, { 1.f, 1.f }, 0.f, 0.f);		
			position.x += ssOffset;
		}
	};
	
	if (m_pPlayer)
	{
		DrawNumbers((int)fabs(m_CurrentLevelTime), { 1280.f / 2.f, 720.f / 4.f });
		DrawNumbers((int)m_pPlayer->GetScore(), { 128.f + 72, 720 - 64 }, false);

		spriteRenderer->Draw(m_pTimingBoard, { 1280.f / 2.f, 720.f / 4.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.5f, 0.5f }, { 2.f, 2.f }, 0.f, 0.f);
		spriteRenderer->Draw(m_pScore, { 64.f, 720 - 128 }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 2.f, 2.f }, 0.f, 0.f);
	}
}
