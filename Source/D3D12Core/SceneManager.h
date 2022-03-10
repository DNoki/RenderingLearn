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
        SceneManager() = delete;

        /**
         * @brief 获取活动场景
         * @return
        */
        inline static Scene* GetActiveScene()
        {
            ASSERT(s_ActiveScene < s_Scenes.size());
            return s_Scenes[s_ActiveScene].get();
        }

        static void Initialize(int startupSceneIndex);

        template <typename T = Scene>
        inline static T& AddScene()
        {
            s_Scenes.push_back(make_unique<T>());
            return *static_cast<T*>(s_Scenes.back().get());
        }

        inline static void LoadNextScene(unsigned int index) { s_NextScene = index; }
        static void CheckLoadNextScene();
        static void Destory();

    private:
        inline static int s_ActiveScene{ 0 };
        inline static int s_NextScene{ -1 };
        inline static std::vector<std::unique_ptr<Scene>> s_Scenes{};
    };
}