#include "stdafx.h"
#include "MonkeyBallMenuScene.h"
#include "MonkeyBallMainScene.h"
#include "ContentManager.h"
#include "SpriteRenderer.h"
#include "TextureData.h"
#include "SceneManager.h"

void MonkeyBallMenuScene::Initialize()
{
	const auto gameContext = GetGameContext();

	m_pBackgroundTexture = ContentManager::Load<TextureData>(L"Resources/MonkeyBall/Texture/mainMenu.png");
	m_pSelectedTexture = ContentManager::Load<TextureData>(L"Resources/MonkeyBall/Texture/selected.png");
	m_pController = ContentManager::Load<TextureData>(L"Resources/MonkeyBall/Texture/controller.png");

	gameContext.pInput->AddInputAction(InputAction(0, InputTriggerState::Released, 'W', -1, XINPUT_GAMEPAD_DPAD_UP));
	gameContext.pInput->AddInputAction(InputAction(1, InputTriggerState::Released, 'S', -1, XINPUT_GAMEPAD_DPAD_DOWN));
	gameContext.pInput->AddInputAction(InputAction(2, InputTriggerState::Released, VK_RETURN, -1, XINPUT_GAMEPAD_A));
}

void MonkeyBallMenuScene::Update()
{
	const auto gameContext = GetGameContext();
	const auto pInput = gameContext.pInput;

	if (pInput->IsActionTriggered(0) || pInput->IsActionTriggered(1))
		m_Position = !m_Position;

	if (pInput->IsActionTriggered(2))
	{
		if (m_Position)
		{
			const auto scene = SceneManager::GetInstance()->GetGameScene(L"MonkeyBallMain");
			static_cast<MonkeyBallMainScene*>(scene)->Reset();

			SceneManager::GetInstance()->SetActiveGameScene(L"MonkeyBallMain");
		}
		else
			PostQuitMessage(WM_QUIT);
	}
}

void MonkeyBallMenuScene::Draw()
{
	const auto spriteRenderer = SpriteRenderer::GetInstance();

	if (m_Position)
		positionSelected = { 32.f, 30.f };
	else
		positionSelected = { 32, 178.f };

	spriteRenderer->Draw(m_pBackgroundTexture, {}, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 1.f, 1.f }, 0.f, 0.1f);
	spriteRenderer->Draw(m_pSelectedTexture, positionSelected, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 1.f, 1.f }, 0.f, 0.f);
	spriteRenderer->Draw(m_pController, { 1280 / 2.f - 128, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 1.f, 1.f }, 0.f, 0.f);
}
