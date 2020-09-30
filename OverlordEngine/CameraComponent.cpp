#include "stdafx.h"
#include "CameraComponent.h"
#include "OverlordGame.h"
#include "TransformComponent.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "GameScene.h"
#include "SceneManager.h"

CameraComponent::CameraComponent() :
	m_FarPlane(10000.0f),
	m_NearPlane(0.1f),
	m_FOV(DirectX::XM_PIDIV4),
	m_Size(25.0f),
	m_IsActive(true),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, DirectX::XMMatrixIdentity());
}

void CameraComponent::Initialize(const GameContext&) {}

void CameraComponent::Update(const GameContext&)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	const auto windowSettings = OverlordGame::GetGameSettings().Window;
	DirectX::XMMATRIX projection;

	if (m_PerspectiveProjection)
	{
		projection = DirectX::XMMatrixPerspectiveFovLH(m_FOV, windowSettings.AspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
		const float viewHeight = (m_Size > 0) ? m_Size : windowSettings.Height;
		projection = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const DirectX::XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const DirectX::XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const DirectX::XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const DirectX::XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const DirectX::XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::Draw(const GameContext&) {}

void CameraComponent::SetActive()
{
	auto gameObject = GetGameObject();
	if (gameObject == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game object is null");
		return;
	}

	auto gameScene = gameObject->GetScene();
	if (gameScene == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game scene is null");
		return;
	}

	gameScene->SetActiveCamera(this);
}

GameObject* CameraComponent::Pick(const GameContext& gameContext, CollisionGroupFlag ignoreGroups) const
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(ignoreGroups);

	auto physxProxy = SceneManager::GetInstance()->GetActiveScene()->GetPhysxProxy();

	// Mouse position, window width and height
	POINT mousePos = gameContext.pInput->GetMousePosition();

	float hWidth = OverlordGame::GetGameSettings().Window.Width / 2.f;
	float hHeight = OverlordGame::GetGameSettings().Window.Height / 2.f;

	float xNdc = (mousePos.x - hWidth) / hWidth;
	float yNdc = (hHeight - mousePos.y) / hHeight;

	// Near and far point
	DirectX::XMMATRIX M_ivp = DirectX::XMLoadFloat4x4(&m_ViewProjectionInverse);
	DirectX::XMFLOAT3 nearp{ xNdc, yNdc, 0.f };
	DirectX::XMVECTOR V_near = DirectX::XMLoadFloat3(&nearp);
	V_near = DirectX::XMVector3TransformCoord(V_near, M_ivp);
	DirectX::XMStoreFloat3(&nearp, V_near);

	DirectX::XMFLOAT3 farp{ xNdc, yNdc, 1.f };
	DirectX::XMVECTOR V_far = DirectX::XMLoadFloat3(&farp);
	V_far = DirectX::XMVector3TransformCoord(V_far, M_ivp);
	DirectX::XMStoreFloat3(&farp, V_far);

	// Raycast
	physx::PxQueryFilterData filterData;
	filterData.data.word0 = ~(physx::PxU32)ignoreGroups;

	physx::PxRaycastBuffer hit;
	auto pos = ToPxVec3(nearp);
	auto dir = ToPxVec3(farp) - pos;
	dir.normalize();

	if (physxProxy->Raycast(pos, dir, PX_MAX_F32, hit, physx::PxHitFlag::eDEFAULT, filterData))
		return reinterpret_cast<BaseComponent*>(hit.block.actor->userData)->GetGameObject();

	//TODO implement
	return nullptr;
}