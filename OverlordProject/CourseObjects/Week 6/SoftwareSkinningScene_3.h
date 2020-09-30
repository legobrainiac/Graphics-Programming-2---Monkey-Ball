#ifndef SOFTWARE_SKINNING_SCENE_3_H
#define SOFTWARE_SKINNING_SCENE_3_H

#include "GameScene.h"
#include "EffectHelper.h"

#include <vector>

struct SkinnedVertex;

class Material;
class BoneObject;
class MeshDrawComponent;

class SoftwareSkinningScene_3 final: public GameScene
{
public:
	SoftwareSkinningScene_3();
	virtual ~SoftwareSkinningScene_3() = default;
    
	SoftwareSkinningScene_3(const SoftwareSkinningScene_3& other) = delete;
	SoftwareSkinningScene_3(SoftwareSkinningScene_3&& other) noexcept = delete;
	SoftwareSkinningScene_3& operator=(const SoftwareSkinningScene_3& other) = delete;
	SoftwareSkinningScene_3& operator=(SoftwareSkinningScene_3&& other) noexcept = delete;
    
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


#endif //SOFTWARE_SKINNING_SCENE_3_H
