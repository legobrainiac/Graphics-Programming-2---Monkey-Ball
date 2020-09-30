#pragma once

class GameObject;
class PhysxProxy;
class SceneManager;
class CameraComponent;
class PostProcessingMaterial;

class GameScene
{
public:
	GameScene(std::wstring sceneName);
	GameScene(const GameScene& other) = delete;
	GameScene(GameScene&& other) noexcept = delete;
	GameScene& operator=(const GameScene& other) = delete;
	GameScene& operator=(GameScene&& other) noexcept = delete;
	virtual ~GameScene();

	void AddChild(GameObject* obj);
	void RemoveChild(GameObject* obj, bool deleteObject = true);

	void MarkDestroy(GameObject* obj);

	const GameContext& GetGameContext() const { return m_GameContext; }

	PhysxProxy* GetPhysxProxy() const { return m_pPhysxProxy; }
	void SetActiveCamera(CameraComponent* pCameraComponent);
	void SetRenderOtherScene(GameScene* pScene) { m_OtherScene = pScene; }

protected:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void SceneActivated() {}
	virtual void SceneDeactivated() {}

	void AddPostProcessingEffect(PostProcessingMaterial* pEffect);
	void RemovePostProcessingEffect(PostProcessingMaterial* pEffect);
	std::vector<PostProcessingMaterial*> m_PostProcessingEffects;

private:
	friend class SceneManager;

	void RootInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void RootUpdate();
	void RootDraw();
	void RootSceneActivated();
	void RootSceneDeactivated();
	void RootWindowStateChanged(int state, bool active) const;

	std::vector<GameObject*> m_pChildren;
	std::vector<GameObject*> m_pDestroyBuffer;
	GameContext m_GameContext;
	bool m_IsInitialized;
	
	GameScene* m_OtherScene = nullptr;

	std::wstring m_SceneName;
	CameraComponent *m_pDefaultCamera, *m_pActiveCamera;
	PhysxProxy* m_pPhysxProxy;
};
