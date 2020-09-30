#ifndef SOFTWARE_SKINNING_SCENE_1_H
#define SOFTWARE_SKINNING_SCENE_1_H

#include "GameScene.h"
#include "EffectHelper.h"

class Material;
class BoneObject;

class SoftwareSkinningScene_1 final: public GameScene
{
    public:
	SoftwareSkinningScene_1();
	virtual ~SoftwareSkinningScene_1() = default;
    
	SoftwareSkinningScene_1(const SoftwareSkinningScene_1& other) = delete;
	SoftwareSkinningScene_1(SoftwareSkinningScene_1&& other) noexcept = delete;
	SoftwareSkinningScene_1& operator=(const SoftwareSkinningScene_1& other) = delete;
	SoftwareSkinningScene_1& operator=(SoftwareSkinningScene_1&& other) noexcept = delete;
    
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
    
private:
    BoneObject* m_pBone0;
    BoneObject* m_pBone1;
    float m_BoneRotation;
    int m_RotationSign;
};

#endif //SOFTWARE_SKINNING_SCENE_1_H
