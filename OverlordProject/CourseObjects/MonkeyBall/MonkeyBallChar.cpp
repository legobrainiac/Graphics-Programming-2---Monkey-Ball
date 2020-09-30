
#include "stdafx.h"
#include "MonkeyBallChar.h"
#include "Components.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "SceneManager.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "TextRenderer.h"
#include "ContentManager.h"
#include "SpriteFont.h"
#include "ModelAnimator.h"
#include "SoundManager.h"
#include "ParticleEmitterComponent.h"
#include "../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "../../Materials/UberMaterial.h"

MonkeyBallChar::MonkeyBallChar(GameObject* pSkyBox)
	: m_pCamera(nullptr)
	, m_RotationY(0.f)
	, m_IntendedRotationY(0.f)
	, m_pSkyBox(pSkyBox)
	, m_Score()
{}

MonkeyBallChar::~MonkeyBallChar()
{
	m_pBubble->release();
}

void MonkeyBallChar::Initialize([[maybe_unused]] const GameContext& gameContext)
{
	// Get Resources
	auto pPhysX = PhysxManager::GetInstance()->GetPhysics();

	m_pDiffuseMaterial = new SkinnedDiffuseMaterial_Shadow();
	m_pDiffuseMaterial->SetDiffuseTexture(L"Resources/MonkeyBall/Texture/textura_mono.jpg");
	m_pDiffuseMaterial->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(m_pDiffuseMaterial, 420);

	// Create camera
	{
		m_pCameraGo = new GameObject();
		m_pFixedCamera = new FixedCamera();
		m_pCameraGo->AddChild(m_pFixedCamera);
		
		GetScene()->AddChild(m_pCameraGo);
	}

	// Monkey model
	{
		m_pModelChild = new GameObject();
		auto pModelComponent = new ModelComponent(L"Resources/MonkeyBall/Meshes/Monkey2.ovm");
		pModelComponent->SetMaterial(420);
		m_pModelChild->AddComponent(pModelComponent);
		m_pModelChild->GetTransform()->Scale({ 0.15f, 0.15f, 0.15f });
		GetScene()->AddChild(m_pModelChild);
	}

	// Sphere model
	{
		auto bol = new GameObject();
		auto pMC = new ModelComponent(L"Resources/MonkeyBall/Meshes/Ball.ovm");
		pMC->SetMaterial(64);
		bol->AddComponent(pMC);
		bol->GetTransform()->Scale({ 2.25f, 2.25f, 2.25f });
		AddChild(bol);
	}

	// Particle Emitter
	{
		m_pParticleObj = new GameObject();
		m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/MonkeyBall/Texture/waterParticle.png", 200);
		m_pParticleEmitter->SetVelocity(DirectX::XMFLOAT3(0, 0.f, 0));
		m_pParticleEmitter->SetMinSize(1.0f);
		m_pParticleEmitter->SetMaxSize(4.0f);
		m_pParticleEmitter->SetMinEnergy(0.5f);
		m_pParticleEmitter->SetMaxEnergy(7.5f);
		m_pParticleEmitter->SetMinSizeGrow(3.5f);
		m_pParticleEmitter->SetMaxSizeGrow(5.5f);
		m_pParticleEmitter->SetMinEmitterRange(0.01f);
		m_pParticleEmitter->SetMaxEmitterRange(0.5f);
		m_pParticleObj->AddComponent(m_pParticleEmitter);
		GetScene()->AddChild(m_pParticleObj);
	}

	// Rigid body
	m_pRigidBody = new RigidBodyComponent();
	AddComponent(m_pRigidBody);

	// Sphere collider
	std::shared_ptr<physx::PxGeometry> pESphere(new physx::PxSphereGeometry(12.5f));
	auto collider = new ColliderComponent(pESphere, *pPhysX->createMaterial(1.0f, 1.0f, 0.1f), physx::PxTransform::createIdentity());
	AddComponent(collider);

	// Action mapping
	gameContext.pInput->AddInputAction(InputAction(MonkeyBallChar::FORWARD, InputTriggerState::Down, 'W', -1, XINPUT_GAMEPAD_DPAD_UP));
	gameContext.pInput->AddInputAction(InputAction(MonkeyBallChar::LEFT, InputTriggerState::Down, 'A', -1, XINPUT_GAMEPAD_DPAD_LEFT));
	gameContext.pInput->AddInputAction(InputAction(MonkeyBallChar::BACKWARD, InputTriggerState::Down, 'S', -1, XINPUT_GAMEPAD_DPAD_DOWN));
	gameContext.pInput->AddInputAction(InputAction(MonkeyBallChar::RIGHT, InputTriggerState::Down, 'D', -1, XINPUT_GAMEPAD_DPAD_RIGHT));
	gameContext.pInput->AddInputAction(InputAction(MonkeyBallChar::JUMP, InputTriggerState::Pressed, VK_SPACE, XINPUT_GAMEPAD_A));

	// Bubble sound
	const auto pFmod = SoundManager::GetInstance()->GetSystem();
	pFmod->createSound("Resources/MonkeyBall/Sound/bubble.wav", FMOD_DEFAULT, nullptr, &m_pBubble);
}

void MonkeyBallChar::PostInitialize([[maybe_unused]] const GameContext& gameContext)
{
	m_pCamera = m_pFixedCamera->GetComponent<CameraComponent>();
	m_pCamera->SetFarClippingPlane(10000.f);
	GetTransform()->Translate({ 0.f, 5.f, 0.f, 0.f });
	m_pCamera->SetActive();

	m_pCamera->SetFieldOfView(XM_PIDIV4);

	//////////////////////////////////////////////////////////////////////////
	m_pAnimator = m_pModelChild->GetComponent<ModelComponent>()->GetAnimator();
	if (m_pAnimator)
	{
		m_pAnimator->Play();
		m_pAnimator->SetAnimationSpeed(2.f);
	}
}

void MonkeyBallChar::Update(const GameContext& gameContext)
{
	using namespace DirectX;

	[[maybe_unused]] const auto pTransform = GetTransform();
	const auto pInput = gameContext.pInput;
	const auto dt = gameContext.pGameTime->GetElapsed();
	const auto forward = m_pCamera->GetTransform()->GetForward();
	const auto right = m_pCamera->GetTransform()->GetRight();
	const auto pFmod = SoundManager::GetInstance()->GetSystem();
	m_BubbleTimer += dt;

	const float speed = 200.f;

	XMFLOAT3 direction{};
	XMFLOAT2 thumb = pInput->GetThumbstickPosition();
	XMFLOAT2 keyAxis = {};

	if (pInput->IsActionTriggered(MonkeyBallChar::FORWARD))
		keyAxis.y += 1.f;

	if (pInput->IsActionTriggered(MonkeyBallChar::BACKWARD))
		keyAxis.y -= 1.f;

	if (pInput->IsActionTriggered(MonkeyBallChar::RIGHT))
		keyAxis.x += 1.f;

	if (pInput->IsActionTriggered(MonkeyBallChar::LEFT))
		keyAxis.x -= 1.f;

	if (fabs(thumb.y) > 0.f)
	{
		direction.z += thumb.y;
		m_pRigidBody->AddForce(ToPxVec3(forward) * thumb.y * speed, physx::PxForceMode::eACCELERATION);
		m_pAnimator->SetPlayReversed(thumb.y < 0.f);
	}
	else if (fabs(keyAxis.y) > 0.f)
	{
		direction.z += keyAxis.y;
		m_pRigidBody->AddForce(ToPxVec3(forward) * keyAxis.y * speed, physx::PxForceMode::eACCELERATION);

		m_pAnimator->SetPlayReversed(keyAxis.y < 0.f);
	}

	if (fabs(thumb.x) > 0.f)
	{
		direction.x += thumb.x;
		m_pRigidBody->AddForce(ToPxVec3(right) * thumb.x * speed, physx::PxForceMode::eACCELERATION);

		m_IntendedRotationY += dt * 4.f * thumb.x;
	}
	else if (fabs(keyAxis.x) > 0.f)
	{
		direction.x += keyAxis.x;
		m_pRigidBody->AddForce(ToPxVec3(right) * keyAxis.x * speed, physx::PxForceMode::eACCELERATION);

		m_IntendedRotationY += dt * 4.f * keyAxis.x;
	}

	// Normalize direction
	//XMVECTOR normalizedDirection = XMVector3Normalize(XMLoadFloat3(&direction));
	XMVECTOR normalizedDirection = XMLoadFloat3(&direction);
	XMStoreFloat3(&m_IntendedDir, normalizedDirection);

	m_RotationY = Lerp(m_RotationY, m_IntendedRotationY, dt * 5.f);

	//InputManager::ForceMouseToCenter(true);
	InputManager::CursorVisible(false);

	// Tilt camera
	m_IntendedTilt = direction.z / 10.f;
	m_CurrentTilt = Lerp(m_CurrentTilt, m_IntendedTilt, dt * 5.f);

	m_IntendedRoll = -direction.x / 5.f;
	m_CurrentRoll = Lerp(m_CurrentRoll, m_IntendedRoll, dt * 5.f);

	m_pCamera->GetTransform()->Rotate(XMQuaternionRotationRollPitchYaw(m_CurrentTilt, 0.f, m_CurrentRoll));

	// Position
	auto playerPosition = pTransform->GetPosition();
	playerPosition.y += m_CurrentTilt * 100.f;
	const auto pCameraTransform = m_pCameraGo->GetTransform();
	auto localPosition = XMVector3TransformNormal(XMLoadFloat3(&m_CameraOffset), XMMatrixRotationY(m_RotationY));
	auto totalCameraPosition = XMLoadFloat3(&playerPosition) + localPosition;
	pCameraTransform->Translate(totalCameraPosition);

	// Rotation
	auto mat = DirectX::XMMatrixLookAtLH(totalCameraPosition, XMLoadFloat3(&GetTransform()->GetWorldPosition()), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	pCameraTransform->Rotate(XMVectorSet(0.f, -1.f, 0.f, 1.f) * XMQuaternionRotationMatrix(mat));

	// Debug information
	auto rotation = m_pCameraGo->GetTransform()->GetWorldRotation();
	auto position = GetTransform()->GetWorldPosition();

	// Set Monkey rotation
	m_pModelChild->GetTransform()->Rotate(XMLoadFloat4(&pCameraTransform->GetRotation()));
	auto pos = pTransform->GetPosition();
	pos.y -= 10.f;
	m_pModelChild->GetTransform()->Translate(pos);

	// Set Monkey animation speed
	float linearVelocity = m_pRigidBody->GetPxRigidBody()->getLinearVelocity().magnitude();
	m_pAnimator->SetAnimationSpeed(linearVelocity / 100.f);

	// Play monkey sound
	if (m_BubbleTimer > 30.f / linearVelocity)
	{
		FMOD::Channel* pChannel = nullptr;
		pFmod->playSound(m_pBubble, nullptr, false, &pChannel);
		pChannel->setVolume(0.1f);
		pChannel->setPitch(randF(0.5f, 1.5f));
		m_BubbleTimer = 0.f;
	}

	// Set Part
	m_pParticleObj->GetTransform()->Translate(pos);

	SkyBoxRotation(dt);

	// Hardcoded map fall off reset
	if (pTransform->GetPosition().y < -200.f)
		ResetPosition();

	//////////////////////////////////////////////////////////////////////////
	gameContext.pShadowMapper->SetLight(
		{
			pTransform->GetPosition().x,
			100.f + pTransform->GetPosition().y,
			pTransform->GetPosition().z
		},
		{ 0.1f, -0.8f, 0.1f }
	);
}

void MonkeyBallChar::Draw([[maybe_unused]] const GameContext& gameContext)
{
}

void MonkeyBallChar::Yeet()
{
	m_pRigidBody->GetPxRigidBody()->setLinearVelocity(physx::PxVec3(0, 500, 0));
}

void MonkeyBallChar::ResetPosition()
{
	GetTransform()->Translate(0.f, 10.f, 0.f);
	m_pRigidBody->GetPxRigidBody()->setLinearVelocity(physx::PxVec3(0, 0, 0));
	m_pRigidBody->GetPxRigidBody()->setAngularVelocity(physx::PxVec3(0, 0, 0));
}

void MonkeyBallChar::SkyBoxRotation([[maybe_unused]] float dt)
{
	//////////////////////////////////////////////////////////////////////////
	auto intentedRotated = XMVector3TransformNormal(XMLoadFloat3(&m_IntendedDir), XMMatrixRotationY(m_RotationY - XM_PIDIV2));
	XMStoreFloat3(&m_CurrentDir, XMVectorLerp(XMLoadFloat3(&m_CurrentDir), intentedRotated, dt * 8.f));

	XMFLOAT3 skyboxRotation{};
	XMStoreFloat3(&skyboxRotation, XMLoadFloat3(&m_CurrentDir) * 0.4f);
	m_pSkyBox->GetTransform()->Rotate(skyboxRotation, false);
	m_pSkyBox->GetTransform()->Translate(m_pCamera->GetTransform()->GetWorldPosition());
}
