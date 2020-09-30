#ifndef BONE_OBJECT_H
#define BONE_OBJECT_H

#include "GameObject.h"
#include "VertexHelper.h"

class ModelComponent;
struct GameContext;

struct SkinnedVertex
{
    SkinnedVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT4 color, float blendWeightBone0, float blendWeightBone1)
        : transformedVertex(position, normal, color)
        , originalVertex(position, normal, color)
        , blendWeightB0(blendWeightBone0)
        , blendWeightB1(blendWeightBone1)
    {
    }
    
    SkinnedVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT4 color)
        : transformedVertex(position, normal, color)
        , originalVertex(position, normal, color)
        , blendWeightB0(0.5f)
        , blendWeightB1(0.5f)
    {
    }
    
    VertexPosNormCol transformedVertex;
    VertexPosNormCol originalVertex;
    
    float blendWeightB0;
    float blendWeightB1;
};

class BoneObject final : public GameObject
{
public:
    BoneObject(int boneId, int materialId, float length = 5.0f);
    ~BoneObject() = default;
    
    BoneObject(const BoneObject& other) = delete;
    BoneObject(BoneObject&& other) noexcept = delete;
    BoneObject& operator=(const BoneObject& other) = delete;
    BoneObject& operator=(BoneObject&& other) noexcept = delete;
    
    void AddBone(BoneObject* pBone);
    
    void CalculateBindPose();
    inline const DirectX::XMFLOAT4X4 GetBindPose() const { return m_BindPose; }
    
protected:
    virtual void Initialize(const GameContext& gameContext);
    
private:
    float m_Length;
    int m_BoneId;
    int m_MaterialId;
    
    DirectX::XMFLOAT4X4 m_BindPose;
    
    ModelComponent* m_pModelComponent;
    GameObject* m_pBone;
};

#endif //BONE_OBJECT_H
