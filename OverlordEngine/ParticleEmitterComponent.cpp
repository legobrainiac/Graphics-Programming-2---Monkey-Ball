#include "stdafx.h"
#include "ParticleEmitterComponent.h"
 #include <utility>
#include "EffectHelper.h"
#include "ContentManager.h"
#include "TextureDataLoader.h"
#include "Particle.h"
#include "TransformComponent.h"

ParticleEmitterComponent::ParticleEmitterComponent(std::wstring  assetFile, int particleCount):
	m_pVertexBuffer(nullptr),
	m_pEffect(nullptr),
	m_pParticleTexture(nullptr),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_Settings(ParticleEmitterSettings()),
	m_ParticleCount(particleCount),
	m_ActiveParticles(0),
	m_LastParticleInit(0.0f),
	m_AssetFile(std::move(assetFile))
{
	//TODO: See Lab9_2
	m_Particles.reserve(m_ParticleCount);
	for (int i = 0; i < m_ParticleCount; ++i)
		m_Particles.emplace_back(new Particle(m_Settings));
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	for (const auto p : m_Particles)
		delete p;

	m_Particles.clear();

	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
	LoadEffect(gameContext);
	CreateVertexBuffer(gameContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"Resources/Effects/ParticleRenderer.fx");
	m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);
	m_pWvpVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	m_pTextureVariable = m_pEffect->GetVariableByName("gParticleTexture")->AsShaderResource();

	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pDefaultTechnique, &m_pInputLayout, m_pInputLayoutSize);
}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
	const auto pDevice = gameContext.pDevice;

	SafeRelease(m_pVertexBuffer);
	D3D11_BUFFER_DESC desc;

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(ParticleVertex) * (uint32_t)m_ParticleCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	const auto result = pDevice->CreateBuffer(&desc, nullptr, &m_pVertexBuffer);
	Logger::LogHResult(result, L"CreateBuffer did the big combust");
}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
	const auto pDeviceContext = gameContext.pDeviceContext;
	const auto dt = gameContext.pGameTime->GetElapsed();

	const auto avgEnergy = (m_Settings.MaxEnergy + m_Settings.MinEnergy) / 2.f;
	const auto particleInterval = avgEnergy / m_ParticleCount;
	m_LastParticleInit += dt;

	// Map the resource and add tings
	m_ActiveParticles = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	const auto pBuffer = static_cast<ParticleVertex*>(mappedResource.pData);

	for (const auto particle : m_Particles)
	{
		particle->Update(gameContext);
		
		if (particle->IsActive())
			pBuffer[m_ActiveParticles++] = particle->GetVertexInfo();
		else if (m_LastParticleInit >= particleInterval)
		{
			particle->Init(GetTransform()->GetWorldPosition());
			pBuffer[m_ActiveParticles++] = particle->GetVertexInfo();
			m_LastParticleInit = 0.f;
		}
	}
	
	// Unmap the resource
	pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::Draw(const GameContext&)
{}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{
	const auto pDeviceContext = gameContext.pDeviceContext;

	const auto fakeWVP = gameContext.pCamera->GetViewProjection();
	const auto viewInverse = gameContext.pCamera->GetViewInverse();

	m_pWvpVariable->SetMatrix(reinterpret_cast<const float*>(&fakeWVP));
	m_pViewInverseVariable->SetMatrix(reinterpret_cast<const float*>(&viewInverse));
	m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());

	pDeviceContext->IASetInputLayout(m_pInputLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	const auto stride = sizeof(ParticleVertex);
	const auto offset = 0U;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pDefaultTechnique->GetDesc(&techDesc);

	for (unsigned int i = 0U; i < techDesc.Passes; ++i)
	{
		m_pDefaultTechnique->GetPassByIndex(i)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}
