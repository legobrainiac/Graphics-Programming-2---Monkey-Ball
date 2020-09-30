#ifndef BANANA_H
#define BANANA_H

#include "GameObject.h"

class DiffuseMaterial_Shadow;
class SkinnedDiffuseMaterial_Shadow;
namespace FMOD { class Sound; class Channel; }

class Banana :
	public GameObject
{
public:
	Banana(DirectX::XMFLOAT3 startPos)
		: m_Rotation()
		, m_PosY()
		, m_Timer()
		, m_GroundOffset(0.f)
		, m_HoverStrenght(5.f)
		, m_HoverSpeed(2.f)
		, m_StartPosition(startPos)
		, m_pBanana(nullptr)
		, m_pMaterial(nullptr)
	{
		++m_BananaCounter;
	}

	virtual ~Banana();

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

private:
	SkinnedDiffuseMaterial_Shadow* m_pMaterial;
	GameObject* m_pBanana;
	GameObject* m_pTrigger;
	
	bool m_HasBeenMonkeyed = false;
	float m_HasBeenMonkeyedTimer = 0.f;

	DirectX::XMFLOAT3 m_StartPosition;
	static FMOD::Sound* m_pChime;
	static int m_BananaCounter;

	float m_Rotation;
	float m_PosY;
	float m_Timer;
	float m_GroundOffset;
	float m_HoverStrenght;
	float m_HoverSpeed;
};

#endif // !BANANA_H
