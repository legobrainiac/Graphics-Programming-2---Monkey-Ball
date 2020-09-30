#include "stdafx.h"
#include "SceneManager.h"
#include "ShadowMapRenderer.h"
#include "ContentManager.h"
#include "ShadowMapMaterial.h"
#include "RenderTarget.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "OverlordGame.h"
#include "SceneManager.h"
#include "GameScene.h"	

ShadowMapRenderer::~ShadowMapRenderer()
{
	//TODO: make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
	SafeDelete(m_pShadowRT);
	SafeDelete(m_pShadowMat);
}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
	if (m_IsInitialized)
		return;

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: create shadow generator material + initialize it
	//TODO: create a render target with the correct settings (hint: depth only) for the shadow generator using a RENDERTARGET_DESC

	m_pShadowMat = new ShadowMapMaterial();
	m_pShadowMat->Initialize(gameContext);
	
	RENDERTARGET_DESC desc{};
	desc.Width = SceneManager::GetInstance()->GetGame()->GetGameSettings().Window.Width;
	desc.Height = SceneManager::GetInstance()->GetGame()->GetGameSettings().Window.Height;
	desc.EnableDepthBuffer = true;
	desc.EnableDepthSRV = true;
	desc.EnableColorBuffer = false;
	desc.EnableColorSRV = false;

	m_pShadowRT = new RenderTarget(gameContext.pDevice);
	m_pShadowRT->Create(desc);

	m_IsInitialized = true;
}

void ShadowMapRenderer::SetLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
{
	using namespace DirectX;
	m_LightPosition = position;
	m_LightDirection = direction;

	const auto w = SceneManager::GetInstance()->GetGame()->GetGameSettings().Window.Width;
	const auto h = SceneManager::GetInstance()->GetGame()->GetGameSettings().Window.Height;

	auto ortho = DirectX::XMMatrixOrthographicLH(m_Size * w / h, m_Size, .1f, m_Far);
	auto lightVP = XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&position) + XMLoadFloat3(&direction), XMVectorSet(0.f, 1.f, 0.f, 0.f)) * ortho;

	XMStoreFloat4x4(&m_LightVP, lightVP);

	//TODO: store the input parameters in the appropriate data members
	//TODO: calculate the Light VP matrix (Directional Light only ;)) and store it in the appropriate datamember
}

void ShadowMapRenderer::Begin(const GameContext& gameContext) const
{
	const auto pDeviceContext = gameContext.pDeviceContext;
	const auto pGame = SceneManager::GetInstance()->GetGame();

	//Reset Texture Register 5 (Unbind)
	ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	// TODO: set the appropriate render target that our shadow generator will write to (hint: use the OverlordGame::SetRenderTarget function through SceneManager)
	pGame->SetRenderTarget(m_pShadowRT);
	
	// TODO: clear this render target
	constexpr float colour[4]{ 0.f, 0.f, 0.f, 0.f };
	m_pShadowRT->Clear(gameContext, colour);
	
	// TODO: set the shader variables of this shadow generator material
	m_pShadowMat->SetLightVP(m_LightVP);
}

void ShadowMapRenderer::End(const GameContext& gameContext) const
{
	UNREFERENCED_PARAMETER(gameContext);

	//TODO: restore default render target (hint: passing nullptr to OverlordGame::SetRenderTarget will do the trick)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	const auto pDeviceContext = gameContext.pDeviceContext;
	ShadowMapMaterial::ShadowGenType genType = ShadowMapMaterial::Static;

	if (pMeshFilter->m_HasAnimations)
		genType = ShadowMapMaterial::ShadowGenType::Skinned;

	//TODO: update shader variables in material
	m_pShadowMat->SetLightVP(m_LightVP);
	m_pShadowMat->SetWorld(world);
	
	// We only give it bones if we're working with a skinned object
	if (genType == ShadowMapMaterial::Skinned)
		m_pShadowMat->SetBones(reinterpret_cast<const float*>(&bones[0]), bones.size());

	//TODO: set the correct input layout, buffers, topology (some variables are set based on the generation type Skinned or Static)
	// Set input layout according to gen type
	pDeviceContext->IASetInputLayout(m_pShadowMat->m_pInputLayouts[genType]);
	
	//////////////////////////////////////////////////////////////////////////
	// Set buffers
	 
	// Vertex buffer
	uint32_t offset = 0;
	const auto vertexBuffer = pMeshFilter->GetVertexBufferData(gameContext, genType/* is this correct? makes sense to me but, smelly*/);
	pDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer.pVertexBuffer, &vertexBuffer.VertexStride, &offset);

	// Index buffer
	pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set topology
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TODO: invoke draw call
	D3DX11_TECHNIQUE_DESC techDesc{};
	const auto pTechnique = m_pShadowMat->m_pShadowTechs[genType];
	pTechnique->GetDesc(&techDesc);

	for (unsigned int j = 0; j < techDesc.Passes; ++j)
	{
		pTechnique->GetPassByIndex(j)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
	}
}

void ShadowMapRenderer::UpdateMeshFilter(const GameContext& gameContext, MeshFilter* pMeshFilter)
{
	ShadowMapMaterial::ShadowGenType genType = ShadowMapMaterial::Static;

	if (pMeshFilter->m_HasAnimations)
		genType = ShadowMapMaterial::ShadowGenType::Skinned;

	//TODO: based on the type (Skinned or Static) build the correct vertex buffers for the MeshFilter (Hint use MeshFilter::BuildVertexBuffer)
	pMeshFilter->BuildVertexBuffer(
		gameContext, 
		genType, 
		m_pShadowMat->m_InputLayoutSizes[genType],
		m_pShadowMat->m_InputLayoutDescriptions[genType]
	); /* this ping pong between objects if nasty, there has to be a cleaner way of doing this xD */
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	//TODO: return the depth shader resource view of the shadow generator render target
	return m_pShadowRT->GetDepthShaderResourceView();
}
