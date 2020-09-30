#include "stdafx.h"
#include "BoneObject.h"
#include "ModelComponent.h"
#include "TransformComponent.h"

BoneObject::BoneObject(int boneId, int materialId, float lenght)
    : m_BoneId(boneId)
    , m_MaterialId(materialId)
    , m_Length(lenght)
{
}

void BoneObject::AddBone(BoneObject* pBone)
{
    pBone->GetTransform()->Translate(m_Length, 0.f, 0.f);
    AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{
    auto pTransform = GetTransform();
    DirectX::XMFLOAT4X4 world = pTransform->GetWorld();
    //                                                              v nullptr since we don't need determinant
    DirectX::XMStoreFloat4x4(&m_BindPose, DirectX::XMMatrixInverse(nullptr, XMLoadFloat4x4(&world)));
    
    for(auto pGo : GetChildren<BoneObject>())
        pGo->CalculateBindPose();
}

void BoneObject::Initialize(const GameContext&)
{
    m_pModelComponent = new ModelComponent(L"./Resources/Meshes/Bone.ovm");
    m_pModelComponent->SetMaterial(m_MaterialId);
    m_pBone = new GameObject();
    m_pBone->AddComponent(m_pModelComponent);
    
    auto pBoneTransform = m_pBone->GetTransform();
    pBoneTransform->Rotate(0.f, -90.f, 0.f);
    pBoneTransform->Scale(m_Length, m_Length, m_Length);
    
    AddChild(m_pBone);
}