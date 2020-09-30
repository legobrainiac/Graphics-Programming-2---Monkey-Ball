#include "stdafx.h"
#include "MonkeyBallEnd.h"
#include "MonkeyBallMainScene.h"
#include "ContentManager.h"
#include "SpriteRenderer.h"
#include "TextureData.h"
#include "SceneManager.h"

void MonkeyBallEnd::Initialize()
{
	const auto gameContext = GetGameContext();

	m_pBackgroundTexture = ContentManager::Load<TextureData>(L"Resources/MonkeyBall/Texture/end.png");
}

void MonkeyBallEnd::Update()
{
	const auto gameContext = GetGameContext();

	m_Timer += gameContext.pGameTime->GetElapsed();

	if (m_Timer > 5.f)
	{
		static_cast<MonkeyBallMainScene*>(SceneManager::GetInstance()->GetGameScene(L"MonkeyBallMain"))->Reset();
		SceneManager::GetInstance()->SetActiveGameScene(L"MonkeyBallMain");
		m_Timer = 0.f;
	}
}

void MonkeyBallEnd::Draw()
{
	const auto spriteRenderer = SpriteRenderer::GetInstance();
	spriteRenderer->Draw(m_pBackgroundTexture, {}, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 1.f, 1.f }, 0.f, 0.1f);
}
