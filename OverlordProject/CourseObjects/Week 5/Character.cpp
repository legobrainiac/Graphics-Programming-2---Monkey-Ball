
#include "stdafx.h"
#include "Character.h"
#include "Components.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "SceneManager.h"

Character::Character(float radius, float height, float moveSpeed) :
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pCamera(nullptr),
	m_pController(nullptr),
	m_TotalPitch(0),
	m_TotalYaw(0),
	m_RotationSpeed(90.f),

	//Running
	m_MaxRunVelocity(50.0f),
	m_TerminalVelocity(2),
	m_Gravity(9.81f),
	m_RunAccelerationTime(0.3f),
	m_JumpAccelerationTime(1.f),
	m_RunAcceleration(m_MaxRunVelocity / m_RunAccelerationTime),
	m_JumpAcceleration(m_Gravity / m_JumpAccelerationTime),
	m_RunVelocity(0),
	m_JumpVelocity(0),
	m_Velocity(0, 0, 0)
{}

void Character::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//TODO: Create controller
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto pBouncyMaterial = physX->createMaterial(.5f, .5f, 0.1f);
	m_pController = new ControllerComponent(pBouncyMaterial);
	AddComponent(m_pController);

	//TODO: Add a fixed camera as child
	m_pFixedCamera = new FixedCamera();
	AddChild(m_pFixedCamera);

	//TODO: Register all Input Actions
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::FORWARD, InputTriggerState::Down, 'W', -1));
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::LEFT, InputTriggerState::Down, 'A', -1));
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::BACKWARD, InputTriggerState::Down, 'S', -1));
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::RIGHT, InputTriggerState::Down, 'D', -1));
	gameContext.pInput->AddInputAction(InputAction(CharacterMovement::JUMP, InputTriggerState::Pressed, VK_SPACE, -1));
}

void Character::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//TODO: Set the camera as active
	// We need to do this in the PostInitialize because child game objects only get initialized after the Initialize of the current object finishes
	m_pCamera = m_pFixedCamera->GetComponent<CameraComponent>();
	m_pCamera->GetTransform()->Translate(0.f, 0.f, 0.f);
	m_pCamera->SetActive();
	GetTransform()->Translate(0.f, 5.f, 0.f);
}

void Character::Update(const GameContext& gameContext)
{
	using namespace DirectX;

	auto pInput = gameContext.pInput;
	XMFLOAT3 movement{};

	XMFLOAT3 forward = m_pCamera->GetTransform()->GetForward();
	XMFLOAT3 right = m_pCamera->GetTransform()->GetRight();

	if (pInput->IsActionTriggered(CharacterMovement::FORWARD))
		XMStoreFloat3(&movement, XMLoadFloat3(&movement) + XMLoadFloat3(&forward));

	if (pInput->IsActionTriggered(CharacterMovement::BACKWARD))
		XMStoreFloat3(&movement, XMLoadFloat3(&movement) + XMLoadFloat3(&forward) * -1.f);

	if (pInput->IsActionTriggered(CharacterMovement::RIGHT))
		XMStoreFloat3(&movement, XMLoadFloat3(&movement) + XMLoadFloat3(&right));

	if (pInput->IsActionTriggered(CharacterMovement::LEFT))
		XMStoreFloat3(&movement, XMLoadFloat3(&movement) + XMLoadFloat3(&right) * -1.f);

	XMStoreFloat3(&movement, XMVector3Normalize(XMLoadFloat3(&movement)));
	float velY = m_Velocity.y;

	// Movement
	if (abs(movement.x) > 0.f || abs(movement.z) > 0.f)
	{
		XMStoreFloat3(&m_Velocity, XMLoadFloat3(&m_Velocity) + XMLoadFloat3(&movement) * gameContext.pGameTime->GetElapsed() * m_RunAcceleration);
		XMFLOAT3 min = { -m_MaxRunVelocity, 0.f , -m_MaxRunVelocity };
		XMFLOAT3 max = { m_MaxRunVelocity, 0.f , m_MaxRunVelocity };
		XMStoreFloat3(&m_Velocity, XMVectorClamp(XMLoadFloat3(&m_Velocity), XMLoadFloat3(&min),XMLoadFloat3(&max)));
	}
	else
	{
		m_Velocity.x = 0.f;
		m_Velocity.z = 0.f;
		m_Velocity.y = 0.f;
	}

	m_Velocity.y = velY;

	// Jumping
	if (!(m_pController->GetCollisionFlags() & physx::PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		m_JumpVelocity -= m_Gravity * gameContext.pGameTime->GetElapsed();
		m_JumpVelocity -= m_JumpAcceleration * gameContext.pGameTime->GetElapsed();
		Clamp(m_JumpVelocity, m_JumpVelocity, -m_TerminalVelocity);
	}
	else if (pInput->IsActionTriggered(CharacterMovement::JUMP))
	{
		m_JumpVelocity = 0.f;
		m_Velocity.y = 200.f;
	}
	else
		m_Velocity.y;

	m_Velocity.y += m_JumpVelocity;

	XMFLOAT3 final{};
	XMStoreFloat3(&final, XMLoadFloat3(&m_Velocity) * gameContext.pGameTime->GetElapsed());
	m_pController->Move(final, 0.f);

	//TODO: Update the character (Camera rotation, Character Movement, Character Gravity)
	const auto mouseMove = InputManager::GetMouseMovement();
	const float sens = 100.f;
	m_TotalPitch += static_cast<float>(mouseMove.x) * gameContext.pGameTime->GetElapsed() * sens;
	m_TotalYaw += static_cast<float>(mouseMove.y) * gameContext.pGameTime->GetElapsed() * sens;

	m_pCamera->GetTransform()->Rotate(m_TotalYaw, m_TotalPitch, 0.f);
}