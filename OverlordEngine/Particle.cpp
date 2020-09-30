#include "stdafx.h"
#include "Particle.h"

// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
using namespace DirectX;

Particle::Particle(const ParticleEmitterSettings& emitterSettings):
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_IsActive(false),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0)
{}

void Particle::Update(const GameContext& gameContext)
{
	if (!m_IsActive)
		return;

	const auto dt = gameContext.pGameTime->GetElapsed();

	// Consume energy and disable particle if life time is over 
	m_CurrentEnergy -= dt;
	if (m_CurrentEnergy < 0.f)
	{
		m_IsActive = false;
		return;
	}

	XMStoreFloat3(
		&m_VertexInfo.Position, 
		XMLoadFloat3(&m_VertexInfo.Position) + XMLoadFloat3(&m_EmitterSettings.Velocity) * dt
	);

	m_VertexInfo.Color = m_EmitterSettings.Color;
	float particleLifePercent = m_CurrentEnergy / m_TotalEnergy;
	m_VertexInfo.Color.w = particleLifePercent * 2.f;

	// Do the big man lerp
	m_VertexInfo.Size = Lerp(m_InitSize, m_InitSize * m_SizeGrow, 1 - particleLifePercent);
}

void Particle::Init(XMFLOAT3 initPosition)
{
	m_IsActive = true;

	const auto randFloat = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	m_TotalEnergy = randFloat;
	m_CurrentEnergy = randFloat;

	XMFLOAT3 direction = { 1.f, 0.f, 0.f };
	const auto randomDistance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);
	const auto rndRMatrix = XMMatrixRotationRollPitchYaw(
		randF(-XM_PI, +XM_PI), 
		randF(-XM_PI, +XM_PI), 
		randF(-XM_PI, +XM_PI)
	);

	const auto rndDirection = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&direction), rndRMatrix));
	XMStoreFloat3(&m_VertexInfo.Position, XMLoadFloat3(&initPosition) + rndDirection * randomDistance);

	const auto minSize = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_VertexInfo.Size = minSize;
	m_InitSize = minSize;

	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);

	m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);
}
