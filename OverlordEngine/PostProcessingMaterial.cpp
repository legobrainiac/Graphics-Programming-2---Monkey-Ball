#include "stdafx.h"
#include "PostProcessingMaterial.h"
#include "RenderTarget.h"
#include "OverlordGame.h"
#include "ContentManager.h"

PostProcessingMaterial::PostProcessingMaterial(std::wstring effectFile, unsigned int renderIndex,
	std::wstring technique)
	: m_IsInitialized(false),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_effectFile(std::move(effectFile)),
	m_InputLayoutID(0),
	m_RenderIndex(renderIndex),
	m_pRenderTarget(nullptr),
	m_pVertexBuffer(nullptr),
	m_pIndexBuffer(nullptr),
	m_NumVertices(0),
	m_NumIndices(0),
	m_VertexBufferStride(0),
	m_pEffect(nullptr),
	m_pTechnique(nullptr),
	m_TechniqueName(std::move(technique))
{
}

PostProcessingMaterial::~PostProcessingMaterial()
{
	//TODO: delete and/or release necessary objects and/or resources
 	SafeDelete(m_pRenderTarget);
 	SafeRelease(m_pVertexBuffer);
 	SafeRelease(m_pIndexBuffer);
 	SafeRelease(m_pInputLayout);
 	m_pInputLayoutDescriptions.clear();
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		const auto pDevice = gameContext.pDevice;

		//1. LoadEffect (LoadEffect(...))
		LoadEffect(gameContext, m_effectFile);

		//2. CreateInputLaytout (CreateInputLayout(...))
		EffectHelper::BuildInputLayout(pDevice, m_pTechnique, &m_pInputLayout, m_pInputLayoutDescriptions,
			m_pInputLayoutSize, m_InputLayoutID);

		//CreateVertexBuffer (CreateVertexBuffer(...)) > As a TriangleStrip (FullScreen Quad)
		CreateVertexBuffer(gameContext);

		//TODO: complete
		//3. Create RenderTarget (m_pRenderTarget)
		//		Take a look at the class, figure out how to initialize/create a RenderTarget Object
		//		GameSettings > OverlordGame::GetGameSettings()

		const auto w = OverlordGame::GetGameSettings().Window.Width;
		const auto h = OverlordGame::GetGameSettings().Window.Height;

		RENDERTARGET_DESC desc{};
		desc.Width = w;
		desc.Height = h;
		desc.EnableColorSRV = true;
		desc.GenerateMipMaps_Color = 1;

		m_pRenderTarget = new RenderTarget(gameContext.pDevice);
		m_pRenderTarget->Create(desc);

		m_IsInitialized = true;
	}
}

bool PostProcessingMaterial::LoadEffect(const GameContext& gameContext, const std::wstring& effectFile)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(effectFile);

	m_pEffect = ContentManager::Load<ID3DX11Effect>(m_effectFile);

	if (!m_TechniqueName.empty())
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::string techString = converter.to_bytes(m_TechniqueName);
		m_pTechnique = m_pEffect->GetTechniqueByName(techString.c_str());
	}
	else
	{
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	}

	LoadEffectVariables();

	//TODO: complete
	//Load Effect through ContentManager
	//Check if m_TechniqueName (default constructor parameter) is set
	// If SET > Use this Technique (+ check if valid)
	// If !SET > Use Technique with index 0

	//Call LoadEffectVariables

	return true;
}

void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRendertarget)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(previousRendertarget);

	const auto pDeviceContext = gameContext.pDeviceContext;

	//TODO: complete
	//1. Clear the object's RenderTarget (m_pRenderTarget) [Check RenderTarget Class]
	constexpr float colour[4]{ 0.f, 0.f, 0.f, 0.f };
	m_pRenderTarget->Clear(gameContext, colour);

	//2. Call UpdateEffectVariables(...)
	UpdateEffectVariables(previousRendertarget);
	
	//3. Set InputLayout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	//4. Set VertexBuffer
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &offset);

	//5. Set PrimitiveTopology (TRIANGLELIST)
	// Do we really want a triangle list though, we're giving it a 4 vertex quad and strip works with that?
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//6. Draw 
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pTechnique->GetDesc(&techDesc);

	for (unsigned int i = 0; i < techDesc.Passes; ++i)
	{
		m_pTechnique->GetPassByIndex(i)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(4, 0);
	}

	// Generate Mips
	gameContext.pDeviceContext->GenerateMips(m_pRenderTarget->GetShaderResourceView());

	ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
}

void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	if (m_pVertexBuffer)
		return;

	m_NumVertices = 4;
	m_VertexBufferStride = sizeof(VertexPosTex);

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_IMMUTABLE; 
	desc.ByteWidth = sizeof(VertexPosTex) * m_NumVertices;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// Grapiks
	// o------------------o
	// | A              B |
	// |             /    |
	// |           /      |
	// |        /         |
	// |      /           |
	// |    /             |
	// | C              D |
	// o------------------o

	VertexPosTex arr[4]
	{
		{ { -1.f, 1.f, 0.f }, { 0.f, 0.f } },
		{ { 1.f, 1.f, 0.f }, { 1.f, 0.f } },
		{ { -1.f, -1.f, 0.f }, { 0.f, 1.f } },
		{ { 1.f, -1.f, 0.f }, { 1.f, 1.f } }
	};

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = reinterpret_cast<void*>(&arr);
	auto hr = gameContext.pDevice->CreateBuffer(&desc, &initData, &m_pVertexBuffer);
	Logger::LogHResult(hr, L"PostProcessingMaterial::CreateVertexBuffer()");

	CreateIndexBuffer(gameContext);

	//TODO: complete
	//Create vertex array containing three elements in system memory
	//fill a buffer description to copy the vertexdata into graphics memory
	//create a ID3D10Buffer in graphics memory containing the vertex info
}

void PostProcessingMaterial::CreateIndexBuffer(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	// BIG MAN, we create index boofer but do not use, wat do?
	//  sBinnalla

	m_NumIndices = 6;

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(DWORD) * m_NumIndices;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	DWORD indices[6] = { 0, 3, 2, 0, 1, 3 }; // Is this even right, I didn't do the big test
	
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = reinterpret_cast<void*>(&indices);

	auto hr = gameContext.pDevice->CreateBuffer(&desc, &initData, &m_pIndexBuffer);
	Logger::LogHResult(hr, L"PostProcessingMaterial::BuildIndexBuffer()");
}
