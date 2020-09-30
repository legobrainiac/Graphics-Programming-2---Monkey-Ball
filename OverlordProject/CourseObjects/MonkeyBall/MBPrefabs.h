#ifndef MB_PREFABS_H
#define MB_PREFABS_H

class GameObject;

class MBPrefabs
{
public:
	static GameObject* SpinningObjectPrefab(uint32_t materialID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotatingSpeed);
	static GameObject* SkyDomePrefab(uint32_t materialID);
	static std::vector<GameObject*> LevelPrefab(GameScene*pGameScene, const std::wstring& name, float& levelTimeOut);
};

#endif // !MB_PREFABS_H