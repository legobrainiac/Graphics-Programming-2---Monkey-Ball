#include "stdafx.h"
#include "RotatorComponent.h"

#include "GameObject.h"
#include "PhysxManager.h"
#include "ModelComponent.h"
#include "ContentManager.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"

void RotatorComponent::Initialize([[maybe_unused]] const GameContext& gameContext)
{
	
}

void RotatorComponent::Update([[maybe_unused]] const GameContext& gameContext)
{
	m_Timer += gameContext.pGameTime->GetElapsed();
	const auto transform = m_pGameObject->GetTransform();

	transform->Rotate(DirectX::XMFLOAT3 { m_Rotation.x * m_Timer, m_Rotation.y * m_Timer, m_Rotation.z * m_Timer }, false);
}

void RotatorComponent::Draw([[maybe_unused]] const GameContext& gameContext)
{
}