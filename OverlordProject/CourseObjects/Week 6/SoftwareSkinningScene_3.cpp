#include "stdafx.h"
#include "BoneObject.h"
#include "MeshDrawComponent.h"
#include "TransformComponent.h"
#include "SoftwareSkinningScene_3.h"
#include "../../Materials/ColorMaterial.h"

SoftwareSkinningScene_3::SoftwareSkinningScene_3()
	: GameScene(L"SoftwareSkinningScene_3")
	, m_pBone0(nullptr)
	, m_pBone1(nullptr)
	, m_BoneRotation(0.f)
	, m_RotationSign(1)
{
}

void SoftwareSkinningScene_3::Initialize()
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

void SoftwareSkinningScene_3::Update()
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

	// ALL THE COOBES, idk i was bored
	for (size_t i = 0; i < m_SkinnedVertices.size(); ++i)
	{
		XMFLOAT4X4 interpTransform{};
		auto percentageT1 = XMLoadFloat4x4(&transformB0) * m_SkinnedVertices[i].blendWeightB0;
		auto percentageT2 = XMLoadFloat4x4(&transformB1) * m_SkinnedVertices[i].blendWeightB1;

		XMStoreFloat4x4(&interpTransform, percentageT1 + percentageT2);

		XMStoreFloat3(&m_SkinnedVertices[i].transformedVertex.Position, XMVector3TransformCoord(XMLoadFloat3(&m_SkinnedVertices[i].originalVertex.Position), XMLoadFloat4x4(&interpTransform)));
	}

	m_pMeshDrawer->RemoveTriangles();
	for (size_t i = 0; i < m_SkinnedVertices.size(); i += 4)
	{
		QuadPosNormCol qpnc(m_SkinnedVertices[i].transformedVertex, m_SkinnedVertices[i + 1].transformedVertex, m_SkinnedVertices[i + 2].transformedVertex, m_SkinnedVertices[i + 3].transformedVertex);
		m_pMeshDrawer->AddQuad(qpnc);
	}

	m_pMeshDrawer->UpdateBuffer();
}

void SoftwareSkinningScene_3::CreateMesh(float length)
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
		offset.y + pos.y, -offset.z + pos.z), norm, col, 1.0f, 0.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 1.0f, 0.0f));

//BACK
	norm = DirectX::XMFLOAT3(0, 0, 1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 1.0f, 0.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col, 1.0f, 0.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -

		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
//TOP
	norm = DirectX::XMFLOAT3(0, 1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 1.0f, 0.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col, 1.0f, 0.0f));
//BOTTOM
	norm = DirectX::XMFLOAT3(0, -1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 1.0f, 0.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col, 1.0f, 0.0f));
//LEFT
	norm = DirectX::XMFLOAT3(-1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 1.0f, 0.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col, 1.0f, 0.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 1.0f, 0.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col, 1.0f, 0.0f));
//RIGHT
	norm = DirectX::XMFLOAT3(1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
//*****
//BOX2*
//*****
	col = DirectX::XMFLOAT4(0, 1, 0, 0.5f);
	pos = DirectX::XMFLOAT3(22.5f, 0, 0);
	//FRONT
	norm = DirectX::XMFLOAT3(0, 0, -1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.0f, 1.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.0f, 1.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
//BACK
	norm = DirectX::XMFLOAT3(0, 0, 1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.0f, 1.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.0f, 1.0f));
//TOP
	norm = DirectX::XMFLOAT3(0, 1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.0f, 1.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.0f, 1.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
//BOTTOM
	norm = DirectX::XMFLOAT3(0, -1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.0f, 1.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.0f, 1.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
//LEFT
	norm = DirectX::XMFLOAT3(-1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.0f, 1.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.0f, 1.0f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.5f, 0.5f));
//RIGHT
	norm = DirectX::XMFLOAT3(1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.0f, 1.f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x,
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.0f, 1.f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, offset.z + pos.z), norm, col, 0.0f, 1.f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -
		offset.y + pos.y, -offset.z + pos.z), norm, col, 0.0f, 1.f));
}

void SoftwareSkinningScene_3::Draw()
{}
