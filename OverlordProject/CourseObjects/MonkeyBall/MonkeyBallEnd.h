#ifndef MONKEY_BALL_END_H
#define MONKEY_BALL_END_H

#include "GameScene.h"

class TextureData;

class MonkeyBallEnd
	: public GameScene
{
public:
	MonkeyBallEnd()
		: GameScene(L"MonkeyBallEnd")
	{
	}

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	TextureData* m_pBackgroundTexture;
	TextureData* m_pSelectedTexture;

	bool m_Position = true;
	float m_Timer = 0.f;

	DirectX::XMFLOAT2 positionSelected{};
};

#endif // !MONKEY_BALL_MENU_SCENE_H
