#ifndef END_PORTAL_H
#define END_PORTAL_H

#include "GameObject.h"

class EndPortal
	: public GameObject
{
public:
	EndPortal(DirectX::XMFLOAT3 pos, float rot);

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

private:
	float m_Timer = 0.f;
	GameObject* m_pTrigger;
	GameObject* m_pModels;
	DirectX::XMFLOAT3 m_Pos;
	float m_Rot;
};

#endif // !END_PORTAL_H