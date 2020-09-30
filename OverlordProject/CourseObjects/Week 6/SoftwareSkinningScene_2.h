#ifndef SOFTWARE_SKINNING_SCENE_2_H
#define SOFTWARE_SKINNING_SCENE_2_H

#include "GameScene.h"
#include "EffectHelper.h"

#include <vector>

struct SkinnedVertex;

class Material;
class BoneObject;
class MeshDrawComponent;

class SoftwareSkinningScene_2 final: public GameScene
{
public:
	SoftwareSkinningScene_2();
	virtual ~SoftwareSkinningScene_2() = default;
    
	SoftwareSkinningScene_2(const SoftwareSkinningScene_2& other) = delete;
	SoftwareSkinningScene_2(SoftwareSkinningScene_2&& other) noexcept = delete;
	SoftwareSkinningScene_2& operator=(const SoftwareSkinningScene_2& other) = delete;
	SoftwareSkinningScene_2& operator=(SoftwareSkinningScene_2&& other) noexcept = delete;
    
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
    
private:
    BoneObject* m_pBone0;
    BoneObject* m_pBone1;
    GameObject* m_pMeshObject;
    
    float m_BoneRotation;
    int m_RotationSign;
    
    void CreateMesh(float length);
    MeshDrawComponent* m_pMeshDrawer;
    std::vector<SkinnedVertex> m_SkinnedVertices;
};


#endif //SOFTWARE_SKINNING_SCENE_2_H
