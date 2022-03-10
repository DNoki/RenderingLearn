#include "pch.h"

#include "GameObject.h"
#include "Transform.h"
#include "SampleScene.h"
#include "GraphicsResource.h"
#include "IGameResource.h"

#include "SceneManager.h"

using namespace std;

namespace Game
{
    void SceneManager::Initialize(int startupSceneIndex)
    {
        s_ActiveScene = startupSceneIndex;

        AddScene<SampleScene>();
        GetActiveScene()->Initialize();
    }

    void SceneManager::CheckLoadNextScene()
    {
        if (s_NextScene >= 0)
        {
            GetActiveScene()->CloseScene();
            s_ActiveScene = s_NextScene;
            s_NextScene = -1;
            GetActiveScene()->Initialize();
        }
    }

    void SceneManager::Destory()
    {
        for (auto& scene : s_Scenes)
            scene->CloseScene();
        s_Scenes.clear();
    }
}