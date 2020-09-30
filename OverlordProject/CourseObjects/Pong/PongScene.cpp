//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "PongScene.h"
#include "Prefabs.h"
#include "Components.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"

PongScene::PongScene(void)
	: GameScene(L"PongScene")
{
}

void PongScene::Initialize()
{
	//////////////////////////////////////////////////////////////////////////
	// Input setup
	auto pInput = GetGameContext().pInput;
	pInput->AddInputAction(InputAction{ InputActions::START_RESET_GAME, InputTriggerState::Pressed, 'R' });
	pInput->AddInputAction(InputAction{ InputActions::P1_PADDLE_UP, InputTriggerState::Down, 'W' });
	pInput->AddInputAction(InputAction{ InputActions::P1_PADDLE_DOWN, InputTriggerState::Down, 'S' });
	pInput->AddInputAction(InputAction{ InputActions::P2_PADDLE_UP, InputTriggerState::Down, VK_UP });
	pInput->AddInputAction(InputAction{ InputActions::P2_PADDLE_DOWN, InputTriggerState::Down, VK_DOWN });

	//////////////////////////////////////////////////////////////////////////
	// Fixed camera
	m_pFixedCamera = new FixedCamera();
	AddChild(m_pFixedCamera);
	auto pCameraComponent = m_pFixedCamera->GetComponent<CameraComponent>();
	pCameraComponent->UseOrthographicProjection();
	pCameraComponent->SetOrthoSize(50.f);
	SetActiveCamera(pCameraComponent);
	m_pFixedCamera->GetTransform()->Translate(DirectX::XMFLOAT3{ 0.f, 50.f, 0.f });
	m_pFixedCamera->GetTransform()->Rotate(90.f, 0.f, 0.f, true);

	auto pPhysX = PhysxManager::GetInstance()->GetPhysics();
	auto pPhysicsMaterial = pPhysX->createMaterial(.0f, .0f, 1.f);

	// Ground
	auto pGo = new GameObject();
	pGo->AddComponent(new RigidBodyComponent(true));

	//////////////////////////////////////////////////////////////////////////
	// Plane geometry
	std::shared_ptr<physx::PxGeometry> pGeometry(new physx::PxPlaneGeometry());
	pGo->AddComponent(new ColliderComponent(pGeometry, *pPhysicsMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0.f, 0.f, 1.f)))));

	AddChild(pGo);

	//////////////////////////////////////////////////////////////////////////
	// Pong ball
	m_pBall = new SpherePrefab(1.f, 25, DirectX::XMFLOAT4{ 0.f, 1.f, 0.f , 1.f });
	m_pBall->GetTransform()->Translate(0.f, 0.f, 0.f);

	// Rigidbody component
	auto pRigidBodyComponent = new RigidBodyComponent();
	m_pBall->AddComponent(pRigidBodyComponent);

	// Geometry
	std::shared_ptr<physx::PxGeometry> pSphereGeometry(new physx::PxSphereGeometry(1.f));
	m_pBall->AddComponent(new ColliderComponent(pSphereGeometry, *pPhysicsMaterial, physx::PxTransform::createIdentity()));

	AddChild(m_pBall);

	//////////////////////////////////////////////////////////////////////////
	// Play field box
	auto CreateCube = [pPhysicsMaterial](const DirectX::XMFLOAT3& pos, float width, float height, float depth, const DirectX::XMFLOAT4& color)
	{
		GameObject* pCube = new CubePrefab(width, height, depth, color);

		// Rigid body component
		auto pRigidBody = new RigidBodyComponent();
		pRigidBody->SetKinematic(true);
		pCube->AddComponent(pRigidBody);

		// Geometry
		std::shared_ptr<physx::PxGeometry> pGeometry(new physx::PxBoxGeometry(width / 2.f, height / 2.f, depth / 2.f));
		pCube->AddComponent(new ColliderComponent(pGeometry, *pPhysicsMaterial, physx::PxTransform::createIdentity()));
		pCube->GetTransform()->Translate(pos);

		return pCube;
	};

	// Vertical walls
	AddChild(CreateCube(DirectX::XMFLOAT3{ 42.f, 1.f, 0.f }, 1.f, 1.f, 45.f, DirectX::XMFLOAT4{}));
	AddChild(CreateCube(DirectX::XMFLOAT3{ -42.f, 1.f, 0.f }, 1.f, 1.f, 45.f, DirectX::XMFLOAT4{}));

	// Horizontal walls
	AddChild(CreateCube(DirectX::XMFLOAT3{ 0.f, 1.f, 22.f }, 83.f, 1.f, 1.f, DirectX::XMFLOAT4{}));
	AddChild(CreateCube(DirectX::XMFLOAT3{ 0.f, 1.f, -22.f }, 83.f, 1.f, 1.f, DirectX::XMFLOAT4{}));

	//////////////////////////////////////////////////////////////////////////
	// Paddles
	m_pPaddelP1 = CreateCube(DirectX::XMFLOAT3{ -35.f, 1.f, 0.f }, 1.f, 1.f, 10.f, DirectX::XMFLOAT4{});
	AddChild(m_pPaddelP1);

	m_pPaddelP2 = CreateCube(DirectX::XMFLOAT3{ 35.f, 1.f, 0.f }, 1.f, 1.f, 10.f, DirectX::XMFLOAT4{});
	AddChild(m_pPaddelP2);

	//////////////////////////////////////////////////////////////////////////
	// Triggers
	
	// Shared Geometry
	std::shared_ptr<physx::PxGeometry> pTriggerGeometry(new physx::PxBoxGeometry(0.5f, 0.5f, 20.f));

	// Shared callback - Reset game if trigger enter event is activated
	auto pTriggerBehaviour = [this](GameObject* pTrigger, GameObject* pOther, GameObject::TriggerAction action)
	{
		(void)pTrigger;

		if (action == GameObject::TriggerAction::ENTER && pOther == m_pBall)
			m_ShouldReset = true;
	};

	// P1
	auto pTriggerP1 = new GameObject();
	auto pTriggerRigidBodyP1 = new RigidBodyComponent();
	pTriggerRigidBodyP1->SetKinematic(true);
	pTriggerP1->AddComponent(pTriggerRigidBodyP1);

	auto pColliderP1 = new ColliderComponent(pTriggerGeometry, *pPhysicsMaterial, physx::PxTransform::createIdentity());
	pTriggerP1->AddComponent(pColliderP1);
	pTriggerP1->GetTransform()->Translate(DirectX::XMFLOAT3{ -40.f, 1.f, 0.f });
	pColliderP1->EnableTrigger(true);
	pTriggerP1->SetOnTriggerCallBack(pTriggerBehaviour);

	AddChild(pTriggerP1);

	// P2
	auto pTriggerP2 = new GameObject();
	auto pTriggerRigidBodyP2 = new RigidBodyComponent();
	pTriggerRigidBodyP2->SetKinematic(true);
	pTriggerP2->AddComponent(pTriggerRigidBodyP2);

	auto pColliderP2 = new ColliderComponent(pTriggerGeometry, *pPhysicsMaterial, physx::PxTransform::createIdentity());
	pTriggerP2->AddComponent(pColliderP2);
	pTriggerP2->GetTransform()->Translate(DirectX::XMFLOAT3{ 40.f, 1.f, 0.f });
	pColliderP2->EnableTrigger(true);
	pTriggerP2->SetOnTriggerCallBack(pTriggerBehaviour);

	AddChild(pTriggerP2);
}

void PongScene::Update()
{
	const auto gameContext = GetGameContext();

	// Game restart
	if (gameContext.pInput->IsActionTriggered(InputActions::START_RESET_GAME))
		ResetGame();

	PlayerInput();

	if (m_ShouldReset)
	{
		ResetGame();
		m_ShouldReset = false;
	}
}

void PongScene::SceneActivated()
{
	Logger::LogInfo(L"////////////////////////////////////");
	Logger::LogInfo(L"R to start/restart the game");
	Logger::LogInfo(L"WS to move paddle for player 1");
	Logger::LogInfo(L"UP and DOWN arrows to move paddle for player 2");
}

void PongScene::ResetGame()
{
	const float speed = 15000;

	auto pTransform = m_pBall->GetTransform();
	auto pRigidBody = m_pBall->GetComponent<RigidBodyComponent>();

	if (pRigidBody)
	{
		// Clear force and clear torque didn't work
		pRigidBody->GetPxRigidBody()->setLinearVelocity({ 0.f, 0.f, 0.f });
		pRigidBody->GetPxRigidBody()->setAngularVelocity({ 0.f, 0.f, 0.f });

		const DirectX::XMFLOAT2 dirs[4]
		{
			DirectX::XMFLOAT2 { 1.f, 1.f },
			DirectX::XMFLOAT2 { 1.f, -1.f },
			DirectX::XMFLOAT2 { -1.f, 1.f },
			DirectX::XMFLOAT2 { -1.f, -1.f }
		};

		int direction = rand() % 4;
		pRigidBody->AddForce({ dirs[direction].x * speed, 0.f, dirs[direction].y * speed });
	}

	if (pTransform)
		pTransform->Translate(0.f, 0.f, 0.f);

	m_pPaddelP1->GetTransform()->Translate(DirectX::XMFLOAT3{ -35.f, 1.f, 0.f });
	m_pPaddelP2->GetTransform()->Translate(DirectX::XMFLOAT3{ 35.f, 1.f, 0.f });
}

void PongScene::PlayerInput()
{
	const auto gameContext = GetGameContext();
	const float clamp = 16.5f;

	// Player 1 input
	{
		auto pTransform = m_pPaddelP1->GetTransform();
		auto position = pTransform->GetPosition();

		if (gameContext.pInput->IsActionTriggered(InputActions::P1_PADDLE_UP))
			position.z += 20.f * gameContext.pGameTime->GetElapsed();

		if (gameContext.pInput->IsActionTriggered(InputActions::P1_PADDLE_DOWN))
			position.z -= 20.f * gameContext.pGameTime->GetElapsed();

		Clamp(position.z, clamp, -clamp);

		pTransform->Translate(position);
	}
	
	// Player 2 input
	{
		auto pTransform = m_pPaddelP2->GetTransform();
		auto position = pTransform->GetPosition();

		if (gameContext.pInput->IsActionTriggered(InputActions::P2_PADDLE_UP))
			position.z += 20.f * gameContext.pGameTime->GetElapsed();

		if (gameContext.pInput->IsActionTriggered(InputActions::P2_PADDLE_DOWN))
			position.z -= 20.f * gameContext.pGameTime->GetElapsed();

		Clamp(position.z, clamp, -clamp);

		pTransform->Translate(position);
	}
}
