#include "stdafx.h"
#include "BoneObject.h"
#include "MeshDrawComponent.h"
#include "TransformComponent.h"
#include "SoftwareSkinningScene_2.h"
#include "../../Materials/ColorMaterial.h"

SoftwareSkinningScene_2::SoftwareSkinningScene_2()
	: GameScene(L"SoftwareSkinningScene_2")
	, m_pBone0(nullptr)
	, m_pBone1(nullptr)
	, m_BoneRotation(0.f)
	, m_RotationSign(1)
{
}

void SoftwareSkinningScene_2::Initialize()
{
	const auto gameContext = GetGameContext();
	auto pColourMaterial = new ColorMaterial();
	gameContext.pMaterialManager->AddMaterial(pColourMaterial, 0);

	// Create bone structure
	GameObject* pRoot = new GameObject();
	m_pBone0 = new BoneObject(0, 0, 15.f);
	m_pBone1 = new BoneObject(1, 0, 15.f);

	AddChild(pRoot);
	pRoot->AddChild(m_pBone0);
	m_pBone0->AddBone(m_pBone1);

	m_pBone0->CalculateBindPose();

	// Create mesh
	m_pMeshObject = new GameObject();
	m_pMeshDrawer = new MeshDrawComponent(24);
	m_pMeshObject->AddComponent(m_pMeshDrawer);
	AddChild(m_pMeshObject);

	CreateMesh(15.f);
}

void SoftwareSkinningScene_2::Update()
{
	using namespace DirectX;

	const auto gameContext = GetGameContext();
	m_BoneRotation += m_RotationSign * 45.f * gameContext.pGameTime->GetElapsed();

	if (m_BoneRotation < -45.f || m_BoneRotation > 45.f)
		m_RotationSign *= -1;

#if 1    
	m_pBone0->GetTransform()->Rotate(0.f, 0.f, m_BoneRotation);
	m_pBone1->GetTransform()->Rotate(0.f, 0.f, -m_BoneRotation * 2.f);
#endif

	auto GetTransform = [](auto* pBone) -> XMFLOAT4X4
	{
		auto bpB = pBone->GetBindPose();
		auto wmB = pBone->GetTransform()->GetWorld();
		XMFLOAT4X4 transformBone{};
		XMStoreFloat4x4(&transformBone, XMLoadFloat4x4(&bpB) * XMLoadFloat4x4(&wmB));

		return transformBone;
	};

	// Get the bind pose of the bones
	auto transformB0 = GetTransform(m_pBone0);
	auto transformB1 = GetTransform(m_pBone1);

	// Cooooooobes
	// First coobe
	for (int i = 0; i < 24; ++i)
	{
		XMStoreFloat3(&m_SkinnedVertices[i].transformedVertex.Position, XMVector3TransformCoord(XMLoadFloat3(&m_SkinnedVertices[i].originalVertex.Position), XMLoadFloat4x4(&transformB0)));
	}

	// Second coobe
	for (int i = 24; i < 48; ++i)
	{
		XMStoreFloat3(&m_SkinnedVertices[i].transformedVertex.Position, XMVector3TransformCoord(XMLoadFloat3(&m_SkinnedVertices[i].originalVertex.Position), XMLoadFloat4x4(&transformB1)));
	}

	m_pMeshDrawer->RemoveTriangles();
	for (int i = 0; i < 48; i += 4)
	{
		QuadPosNormCol qpnc(m_SkinnedVertices[i].transformedVertex, m_SkinnedVertices[i + 1].transformedVertex, m_SkinnedVertices[i + 2].transformedVertex, m_SkinnedVertices[i + 3].transformedVertex);
		m_pMeshDrawer->AddQuad(qpnc);
	}

	m_pMeshDrawer->UpdateBuffer();
}

void SoftwareSkinningScene_2::CreateMesh(float length)
{
	auto pos = DirectX::XMFLOAT3(length / 2, 0, 0);
	auto offset = DirectX::XMFLOAT3(length / 2, 2.5f, 2.5f);
	auto col = DirectX::XMFLOAT4(1, 0, 0, 0.5f);
	//*****
	//BOX1*
	//*****
	//FRONT
	auto norm = DirectX::XMFLOAT3(0, 0, -1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
//BACK
	norm = DirectX::XMFLOAT3(0, 0, 1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
//TOP
	norm = DirectX::XMFLOAT3(0, 1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
//BOTTOM
	norm = DirectX::XMFLOAT3(0, -1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
//LEFT
	norm = DirectX::XMFLOAT3(-1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
//RIGHT
	norm = DirectX::XMFLOAT3(1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
//*****
//BOX2*
//*****
	col = DirectX::XMFLOAT4(0, 1, 0, 0.5f);
	pos = DirectX::XMFLOAT3(22.5f, 0, 0);
	//FRONT
	norm = DirectX::XMFLOAT3(0, 0, -1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
//BACK
	norm = DirectX::XMFLOAT3(0, 0, 1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
//TOP
	norm = DirectX::XMFLOAT3(0, 1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
//BOTTOM
	norm = DirectX::XMFLOAT3(0, -1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
//LEFT
	norm = DirectX::XMFLOAT3(-1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
//RIGHT
	norm = DirectX::XMFLOAT3(1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col));
}

void SoftwareSkinningScene_2::Draw()
{}
