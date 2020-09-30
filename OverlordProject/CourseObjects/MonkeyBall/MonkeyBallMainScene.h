#ifndef _CHARACTER_SCENE_H
#define _CHARACTER_SCENE_H

#include "GameScene.h"

class PostLUT;
class SpriteFont;
class DiffuseMaterial_Shadow;
class MonkeyBallChar;
class TextureData;
namespace FMOD { class Sound; class Channel; }

class MonkeyBallMainScene final 
	: public GameScene
{
public:
	MonkeyBallMainScene();
	virtual ~MonkeyBallMainScene();

	MonkeyBallMainScene(const MonkeyBallMainScene& other) = delete;
	MonkeyBallMainScene(MonkeyBallMainScene&& other) noexcept = delete;
	MonkeyBallMainScene& operator=(const MonkeyBallMainScene& other) = delete;
	MonkeyBallMainScene& operator=(MonkeyBallMainScene&& other) noexcept = delete;

	void LoadLevel(const std::wstring& level);
	void ReloadCurrentLevel();
	void QueueNextLevel() { m_LoadNext = true; }

	void Reset();

protected:
	void LoadSequentialLevel();
	enum Levels { WORLD1, WORLD2, WORLD3, WORLDCOUNT };
	std::wstring m_Levels[Levels::WORLDCOUNT]{ L"World1", L"World2", L"World3" };
	uint32_t m_WorldIndex = 0U;
	bool m_LoadNext = false;
	float m_LoadTimer = 0.f;

	MonkeyBallChar* m_pPlayer = nullptr;
	DiffuseMaterial_Shadow* m_pShadowMat;
	PostLUT* m_pPost;
	bool m_PostProcessing;

	std::vector<GameObject*> m_CurrentSceneObjects;
	float m_CurrentLevelTime = 120.f;

	TextureData* m_pTimingBoard = nullptr;
	TextureData* m_pScore = nullptr;
	TextureData* m_Numbers[10];
	FMOD::Sound* m_pOST = nullptr;
	FMOD::Sound* m_pWhistle = nullptr;
	FMOD::Channel* pChannel = nullptr;

	void Initialize() override;
	void Update() override;
	void Draw() override;
};

#endif //_CHARACTER_SCENE_H
