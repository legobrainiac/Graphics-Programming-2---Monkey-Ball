#ifndef MONKEY_BALL_IN_GAME_MENU_SCENE_H
#define MONKEY_BALL_IN_GAME_MENU_SCENE_H

#include "GameScene.h"

class TextureData;


class MonkeyBallInGameMenuScene 
	: public GameScene
{
public:
	MonkeyBallInGameMenuScene()
		: GameScene(L"MonkeyBallInGameMenuScene")
	{
	}

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	TextureData* m_pBackgroundTexture;
	TextureData* m_pSelectedTexture;

	bool m_Position = true;

	DirectX::XMFLOAT2 positionSelected{};
};

#endif // !MONKEY_BALL_IN_GAME_MENU_SCENE_H
