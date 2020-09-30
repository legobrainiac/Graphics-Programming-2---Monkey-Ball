#pragma once
#include "GameScene.h"

class FixedCamera;

class PongScene final : public GameScene
{
public:
	PongScene();
	virtual ~PongScene() = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override {}
	void SceneActivated() override;

	void ResetGame();
	void PlayerInput();

	FixedCamera* m_pFixedCamera = nullptr;
	GameObject* m_pBall = nullptr;
	GameObject* m_pPaddelP1 = nullptr;
	GameObject* m_pPaddelP2 = nullptr;

	bool m_ShouldReset = false;

	enum InputActions
	{
		START_RESET_GAME,
		P1_PADDLE_UP,
		P1_PADDLE_DOWN,
		P2_PADDLE_UP,
		P2_PADDLE_DOWN
	};
};


