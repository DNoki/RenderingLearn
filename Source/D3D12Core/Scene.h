#pragma once

namespace Graphics
{
    class GraphicsResource;
}

namespace Game
{
    class Component;
    class GameObject;
    class IGameResource;

    /**
     * @brief 场景
    */
    class Scene
    {
    public:
        std::string m_Name;

        Scene() = default;
        Scene(const Scene & scene) = delete;
        Scene(Scene && scene) = default;

        inline Scene& operator = (const Scene & scene) = delete;
        inline Scene& operator = (Scene && scene) = default;

        /**
         * @brief 添加游戏对象
         * @param gameObj
         * @return
        */
        GameObject& AddGameObject(std::unique_ptr<GameObject>&& gameObj);

        /**
         * @brief 添加游戏对象
         * @param gameObj
         * @return
        */
        GameObject& AddGameObject(std::unique_ptr<GameObject>& gameObj);
        /**
         * @brief 查找并返回场景内找到的第一个已激活对象(仅检索根对象)
         * @param name
         * @return
        */
        GameObject* FindGameObject(const std::string& name) const;

        virtual void Initialize() = 0;
        virtual void ExecuteScene();
        virtual void CloseScene();

    private:
        std::map<UINT64, std::vector<std::unique_ptr<IGameResource>>> m_Resources;
        std::vector<std::unique_ptr<GameObject>> m_HierarchyObject;

        virtual void ExecuteUpdate() = 0;
        virtual void ExecuteRender() = 0;
    };

}