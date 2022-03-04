#pragma once

namespace Game
{
    class Scene;

    /**
     * @brief 场景管理器
    */
    class SceneManager final
    {
    public:
        /**
         * @brief 获取活动场景
         * @return
        */
        inline static Scene* GetActiveScene()
        {
            ASSERT(m_ActiveScene < m_Scenes.size());
            return m_Scenes[m_ActiveScene].get();
        }

        static void Initialize(int startupSceneIndex);

        template <typename T = Scene>
        inline static T& AddScene()
        {
            m_Scenes.push_back(make_unique<T>());
            return *static_cast<T*>(m_Scenes.back().get());
        }

        inline static void LoadNextScene(unsigned int index) { m_NextScene = index; }
        static void CheckLoadNextScene();
        static void Destory();

    private:
        static int m_ActiveScene;
        static int m_NextScene;
        static std::vector<std::unique_ptr<Scene>> m_Scenes;
    };
}