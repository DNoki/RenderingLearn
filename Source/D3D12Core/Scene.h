#pragma once

namespace Graphics
{
    class GraphicsResource;
    class CommandList;
}

namespace Game
{
    class Component;
    class GameObject;
    class IGameResource;

    class VersionedGameObject
    {
    public:
        UINT64 m_Version{ 0 };
        std::shared_ptr<GameObject> m_GameObject{};

        VersionedGameObject() = default;

    private:

    };

    /**
     * @brief 场景
    */
    class Scene
    {
    public:
        std::wstring m_Name{};

        Scene() = default;
        virtual ~Scene() = default;
        Scene(const Scene& scene) = delete;
        Scene(Scene&& scene) = default;

        inline Scene& operator = (const Scene& scene) = delete;
        inline Scene& operator = (Scene&& scene) = default;

        template<typename T = IGameResource>
        T& AddGameResource(std::unique_ptr<T>&& res)
        {
            UINT64 typeHash = typeid(T).hash_code();
            if (m_ResourceMap.find(typeid(T).hash_code()) == m_ResourceMap.end())
            {
                m_ResourceMap.emplace(typeHash, std::vector<std::unique_ptr<IGameResource>>());
            }
            T& result = *res;
            m_ResourceMap[typeHash].push_back(move(res));
            return result;
        }
        template<typename T = IGameResource>
        T& AddGameResource() { return AddGameResource(std::make_unique<T>()); }
        template<typename T = IGameResource>
        T* FindGameResource(const std::wstring& name = L"")
        {
            IGameResource* result = nullptr;
            auto resourcesIt = m_ResourceMap.find(typeid(T).hash_code());
            if (resourcesIt != m_ResourceMap.end() && (resourcesIt->second.size() > 0))
            {
                auto& resources = resourcesIt->second;
                if (!name.empty())
                {
                    for (auto& res : resources)
                    {
                        if (res->GetName() == name)
                        {
                            result = res.get();
                            break;
                        }
                    }
                }
                else result = resources.front().get(); // 不指定名称时则返回第一个资源
            }
            return static_cast<T*>(result);
        }

        /**
         * @brief 添加游戏对象
         * @param gameObj
         * @return
        */
        GameObject& AddGameObject(std::unique_ptr<GameObject>&& gameObj);
        /**
         * @brief 查找并返回场景内找到的第一个已激活对象(仅检索根对象)
         * @param name
         * @return
        */
        GameObject* FindGameObject(const std::wstring& name) const;
        bool RemoveGameObject(const GameObject* obj);

        virtual void Initialize() = 0;
        virtual void ExecuteScene();
        virtual void CloseScene();

    protected:
        std::map<UINT64, std::vector<std::unique_ptr<IGameResource>>> m_ResourceMap{};
        std::vector<VersionedGameObject> m_HierarchyObject{};
        std::map<UINT64, std::vector<Component*>> m_BakedComponents{};

        bool m_IsComponentChanged = false;

        virtual void ExecuteUpdate() = 0;
        virtual void ExecutePrevRender();
        virtual void ExecuteRender();

    private:
        void RenderPass1(std::vector<Graphics::CommandList*>& commandListArray);
        void RenderPass2(std::vector<Graphics::CommandList*>& commandListArray);
        void RenderPass3(std::vector<Graphics::CommandList*>& commandListArray);

    };
}