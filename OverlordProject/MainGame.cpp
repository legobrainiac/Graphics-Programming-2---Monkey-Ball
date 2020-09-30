
#include "stdafx.h"

#include "MainGame.h"
#include "GeneralStructs.h"
#include "SceneManager.h"
#include "PhysxProxy.h"
#include "DebugRenderer.h"

//#define W3
//#define W4_B
//#define W4_A
//#define W5_B
//#define W5_A
//#define W6
//#define W7
//#define W8
//#define W9
//#define W10
#define MONKEY

#ifdef W4_B
#include "CourseObjects/Week 4/ModelTestScene.h"
#include "CourseObjects/Week 4/SpikeyScene.h"
#include "CourseObjects/Week 4/UberScene.h"
#endif

#ifdef W4_A
#include "CourseObjects/Week 4/SpriteTestScene.h"
#endif

#ifdef W3
#include "CourseObjects/Week 3/MinionScene.h"
#include "CourseObjects/Week 3/ComponentTestScene.h"
#include "CourseObjects/Pong/PongScene.h"
#endif

#ifdef W5_A
#include "CourseObjects/Week 5/FontTestScene.h"
#endif

#ifdef W5_B
#include "CourseObjects/Week 5/PickScene.h"
#include "CourseObjects/Week 5/CharacterScene.h"
#endif

#ifdef W6
#include "CourseObjects/Week 6/SoftwareSkinningScene_1.h"
#include "CourseObjects/Week 6/SoftwareSkinningScene_2.h"
#include "CourseObjects/Week 6/SoftwareSkinningScene_3.h"
#endif

#ifdef W7
#include "CourseObjects/Week 7/HardwareSkinningScene.h"
#endif

#ifdef W8
	#include "CourseObjects/Week 8/ShadowMappingScene.h"
#endif

#ifdef W9
	#include "CourseObjects/Week 9/ParticleScene.h"
#endif

#ifdef W10
	#include "CourseObjects/Week 10/PostProcessingScene.h"
#endif

#ifdef MONKEY
#include "CourseObjects/MonkeyBall/MonkeyBallMainScene.h"
#include "CourseObjects/MonkeyBall/MonkeyBallMenuScene.h"
#include "CourseObjects/MonkeyBall/MonkeyBallInGameMenuScene.h"
#include "CourseObjects/MonkeyBall/MonkeyBallEnd.h"
#endif

MainGame::MainGame(void)
{}

MainGame::~MainGame(void)
{}

//Game is preparing
void MainGame::OnGamePreparing(GameSettings& gameSettings)
{
	UNREFERENCED_PARAMETER(gameSettings);
	//Nothing to do atm.
}

void MainGame::Initialize()
{
#ifdef W4_A
	SceneManager::GetInstance()->AddGameScene(new SpriteTestScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"SpriteTestScene");
#endif
#ifdef W4_B
	SceneManager::GetInstance()->AddGameScene(new ModelTestScene());
	SceneManager::GetInstance()->AddGameScene(new SpikeyScene());
	SceneManager::GetInstance()->AddGameScene(new UberScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"SpikeyScene");
#endif
#ifdef W3
	SceneManager::GetInstance()->AddGameScene(new ComponentTestScene());
	SceneManager::GetInstance()->AddGameScene(new PongScene());

	SceneManager::GetInstance()->SetActiveGameScene(L"PongScene");
#endif

#ifdef W5_B
    SceneManager::GetInstance()->AddGameScene(new CharacterScene());
	SceneManager::GetInstance()->AddGameScene(new PickScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"CharacterScene");
#endif

#ifdef W5_A
	SceneManager::GetInstance()->AddGameScene(new FontTestScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"FontTestScene");
#endif
    
#ifdef W6
    SceneManager::GetInstance()->AddGameScene(new SoftwareSkinningScene_1());
    SceneManager::GetInstance()->AddGameScene(new SoftwareSkinningScene_2());
    SceneManager::GetInstance()->AddGameScene(new SoftwareSkinningScene_3());
	SceneManager::GetInstance()->SetActiveGameScene(L"SoftwareSkinningScene_1");
#endif

#ifdef W7
	SceneManager::GetInstance()->AddGameScene(new HardwareSkinningScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"HardwareSkinningScene");
#endif

#ifdef W8
	SceneManager::GetInstance()->AddGameScene(new ShadowMappingScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"ShadowMappingScene");
#endif

#ifdef W9
	SceneManager::GetInstance()->AddGameScene(new ParticleScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"ParticleScene");
#endif

#ifdef W10
	SceneManager::GetInstance()->AddGameScene(new PostProcessingScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"PostProcessingScene");
#endif

#ifdef MONKEY
	SceneManager::GetInstance()->AddGameScene(new MonkeyBallMainScene());
	SceneManager::GetInstance()->AddGameScene(new MonkeyBallMenuScene());
	SceneManager::GetInstance()->AddGameScene(new MonkeyBallInGameMenuScene());
	SceneManager::GetInstance()->AddGameScene(new MonkeyBallEnd());
	SceneManager::GetInstance()->SetActiveGameScene(L"MonkeyBallMainMenu");
#endif
}

LRESULT MainGame::WindowProcedureHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_KEYUP:
	{
		if ((lParam & 0x80000000) != 0x80000000)
			return -1;

		//NextScene
		if (wParam == VK_F3)
		{
			SceneManager::GetInstance()->NextScene();
			return 0;
		}
		//PreviousScene
		else if (wParam == VK_F2)
		{
			SceneManager::GetInstance()->PreviousScene();
			return 0;
		}
		else if (wParam == VK_F4)
		{
			DebugRenderer::ToggleDebugRenderer();
			return 0;
		}
		else if (wParam == VK_F6)
		{
			auto activeScene = SceneManager::GetInstance()->GetActiveScene();
			activeScene->GetPhysxProxy()->NextPhysXFrame();
		}
	}
	}

	return -1;
}
