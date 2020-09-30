#pragma once
#include "GameScene.h"

class ComponentTestScene final : public GameScene
{
public:
	ComponentTestScene();
	virtual ~ComponentTestScene() = default;

	ComponentTestScene(const ComponentTestScene& other) = delete;
	ComponentTestScene(ComponentTestScene&& other) noexcept = delete;
	ComponentTestScene& operator=(const ComponentTestScene& other) = delete;
	ComponentTestScene& operator=(ComponentTestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	float m_FpsInterval = 0.f;
};


