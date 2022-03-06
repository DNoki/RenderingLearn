#include "pch.h"

#include "GameObject.h"
#include "Transform.h"
#include "SampleScene.h"
#include "GraphicsResource.h"

#include "SceneManager.h"

using namespace std;

namespace Game
{
    void SceneManager::Initialize(int startupSceneIndex)
    {
        m_ActiveScene = startupSceneIndex;

        AddScene<SampleScene>();
        GetActiveScene()->Initialize();
    }

    void SceneManager::CheckLoadNextScene()
    {
        if (m_NextScene >= 0)
        {
            GetActiveScene()->CloseScene();
            m_ActiveScene = m_NextScene;
            m_NextScene = -1;
            GetActiveScene()->Initialize();
        }
    }

    void SceneManager::Destory()
    {
        for (auto& scene : m_Scenes)
            scene->CloseScene();
        m_Scenes.clear();
    }
}