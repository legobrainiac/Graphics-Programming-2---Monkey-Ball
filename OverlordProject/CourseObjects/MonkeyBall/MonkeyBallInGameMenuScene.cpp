#include "stdafx.h"
#include "MonkeyBallInGameMenuScene.h"
#include "ContentManager.h"
#include "SpriteRenderer.h"
#include "TextureData.h"
#include "SceneManager.h"

void MonkeyBallInGameMenuScene::Initialize()
{
	const auto gameContext = GetGameContext();

	m_pBackgroundTexture = ContentManager::Load<TextureData>(L"Resources/MonkeyBall/Texture/inGameMenu.png");
	m_pSelectedTexture = ContentManager::Load<TextureData>(L"Resources/MonkeyBall/Texture/selected.png");

	gameContext.pInput->AddInputAction(InputAction(0, InputTriggerState::Released, 'W', -1, XINPUT_GAMEPAD_DPAD_UP));
	gameContext.pInput->AddInputAction(InputAction(1, InputTriggerState::Released, 'S', -1, XINPUT_GAMEPAD_DPAD_DOWN));
	gameContext.pInput->AddInputAction(InputAction(2, InputTriggerState::Released, VK_RETURN, -1, XINPUT_GAMEPAD_A));
	gameContext.pInput->AddInputAction(InputAction(3, InputTriggerState::Released, VK_ESCAPE, -1, XINPUT_GAMEPAD_START));
}

void MonkeyBallInGameMenuScene::Update()
{
	const auto gameContext = GetGameContext();
	const auto pInput = gameContext.pInput;

	if (pInput->IsActionTriggered(0) || pInput->IsActionTriggered(1))
		m_Position = !m_Position;

	if (pInput->IsActionTriggered(2))
	{
		if (m_Position)
			SceneManager::GetInstance()->SetActiveGameScene(L"MonkeyBallMainMenu");
		else
			PostQuitMessage(WM_QUIT);
	}
	
	if (pInput->IsActionTriggered(3))
		SceneManager::GetInstance()->SetActiveGameScene(L"MonkeyBallMain");
}

void MonkeyBallInGameMenuScene::Draw()
{
	const auto spriteRenderer = SpriteRenderer::GetInstance();

	if (m_Position)
		positionSelected = { 32.f, 30.f };
	else
		positionSelected = { 32, 178.f };

	spriteRenderer->Draw(m_pBackgroundTexture, {}, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 1.f, 1.f }, 0.f, 0.1f);
	spriteRenderer->Draw(m_pSelectedTexture, positionSelected, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 1.f, 1.f }, 0.f, 0.f);
}
