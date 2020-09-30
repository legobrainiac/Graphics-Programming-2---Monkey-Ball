#pragma once

#include "GameObject.h"

class SpriteFont;
class FixedCamera;
class ModelAnimator;
class CameraComponent;
class RigidBodyComponent;
class ControllerComponent;
class DiffuseMaterial_Shadow;
class ParticleEmitterComponent;
class SkinnedDiffuseMaterial_Shadow;
namespace FMOD { class Sound; class Channel; }

class MonkeyBallChar : public GameObject
{
public:
	enum CharacterMovement : UINT
	{
		LEFT = 0,
		RIGHT,
		FORWARD,
		BACKWARD,
		JUMP
	};

	MonkeyBallChar(GameObject* pSkyBox);
	virtual ~MonkeyBallChar();

	MonkeyBallChar(const MonkeyBallChar& other) = delete;
	MonkeyBallChar(MonkeyBallChar&& other) noexcept = delete;
	MonkeyBallChar& operator=(const MonkeyBallChar& other) = delete;
	MonkeyBallChar& operator=(MonkeyBallChar&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

	void Draw(const GameContext& gameContext) override;

	CameraComponent* GetCamera() const { return m_pCamera; }

	inline void AddScore(int score) { m_Score += score; }
	[[nodiscard]] constexpr auto GetScore() noexcept -> int { return m_Score; }

	void ResetPosition();

	void Yeet();

protected:
	GameObject* m_pModelChild;
	ModelAnimator* m_pAnimator;

	// This is preety dirty
	CameraComponent* m_pCamera;
	FixedCamera* m_pFixedCamera;
	GameObject* m_pCameraGo;
	float m_CurrentTilt = 0.f;
	float m_IntendedTilt = 0.f;

	float m_IntendedRoll = 0.f;
	float m_CurrentRoll = 0.f;

	SkinnedDiffuseMaterial_Shadow* m_pDiffuseMaterial;
	RigidBodyComponent* m_pRigidBody;

	//////////////////////////////////////////////////////////////////////////
	GameObject* m_pSkyBox;

	GameObject* m_pParticleObj;
	ParticleEmitterComponent* m_pParticleEmitter;

	float m_IntendedRotationY;
	float m_RotationY;
	DirectX::XMFLOAT3 m_CameraOffset = { 0.f, 30.f, -125.f };
	DirectX::XMFLOAT3 m_IntendedDir{};
	DirectX::XMFLOAT3 m_CurrentDir{};

	FMOD::Sound* m_pBubble = nullptr;
	float m_BubbleTimer = 0.f;

	int m_Score;

	void SkyBoxRotation(float dt);
};


