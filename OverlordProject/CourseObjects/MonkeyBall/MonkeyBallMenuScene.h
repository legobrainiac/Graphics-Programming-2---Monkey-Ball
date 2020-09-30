#ifndef MONKEY_BALL_MENU_SCENE_H
#define MONKEY_BALL_MENU_SCENE_H

#include "GameScene.h"

class TextureData;


class MonkeyBallMenuScene 
	: public GameScene
{
public:
	MonkeyBallMenuScene()
		: GameScene(L"MonkeyBallMainMenu")
	{
	}

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	TextureData* m_pBackgroundTexture;
	TextureData* m_pSelectedTexture;
	TextureData* m_pController;

	bool m_Position = true;

	DirectX::XMFLOAT2 positionSelected{};
};

#endif // !MONKEY_BALL_MENU_SCENE_H
