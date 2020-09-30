#ifndef _CHARACTER_SCENE_H
#define _CHARACTER_SCENE_H

#include "GameScene.h"

class CharacterScene final : public GameScene
{
public:
	CharacterScene();
	virtual ~CharacterScene() = default;

	CharacterScene(const CharacterScene& other) = delete;
	CharacterScene(CharacterScene&& other) noexcept = delete;
	CharacterScene& operator=(const CharacterScene& other) = delete;
	CharacterScene& operator=(CharacterScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

#endif //_CHARACTER_SCENE_H
