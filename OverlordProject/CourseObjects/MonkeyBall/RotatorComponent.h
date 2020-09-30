#ifndef ROTATOR_COMPONENT_H
#define ROTATOR_COMPONENT_H

#include "BaseComponent.h"

class RotatorComponent
	: public BaseComponent
{
public:
	inline RotatorComponent(const DirectX::XMFLOAT3& rotation)
		: m_Rotation(rotation)
		, m_Timer()
	{
	}

	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;
	virtual void Draw(const GameContext& gameContext) override;

private:
	float m_Timer;
	DirectX::XMFLOAT3 m_Rotation;
};

#endif // !ROTATOR_COMPONENT_H

