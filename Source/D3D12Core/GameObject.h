#pragma once

namespace Game
{
    class Component;
    class Transform;

    class GameObject final
    {
    public:
        std::wstring m_Name;

        GameObject();
        GameObject(const std::wstring& name, Transform* parent = nullptr, bool worldPositionStays = true);
        GameObject(const GameObject& obj) = delete;
        GameObject(GameObject&& obj) = default;

        inline GameObject& operator = (const GameObject& obj) = delete;
        inline GameObject& operator = (GameObject&& obj) = default;

        /**
         * @brief 是否处于激活状态
         * @param isInHierarchy 在场景中
         * @return
        */
        bool GetActive(bool isInHierarchy = true) const;
        /**
         * @brief 设置激活状态
         * @param value
        */
        void SetActive(bool value) { m_Enabled = value; }
        /**
         * @brief 获取变换
         * @return
        */
        inline Transform& GetTransform() const { return *(m_Transform); }
        inline UINT64 GetVersion() const { return m_Version; }

        /**
         * @brief 添加组件
         * @tparam T
         * @return
        */
        template <class T = Component>
        T& AddComponent()
        {
            if (m_Version == 0) throw L"ERROR::Object has been destroyed.";
            auto component = std::make_unique<T>(*this);

            T* result = component.get();
            m_Components[typeid(T).hash_code()].push_back(move(component));
            m_Version++;

            return *result;
        }
        /**
         * @brief 返回找到的第一个组件
         * @tparam T 组件类型
         * @return
        */
        template <typename T = Component>
        T* GetComponent() const
        {
            T* result = nullptr;
            auto componentListIt = m_Components.find(typeid(T).hash_code());
            if (componentListIt != m_Components.end())
            {
                if (componentListIt->second.size() > 0)
                {
                    result = static_cast<T*>(componentListIt->second.front().get());
                }
            }
            return result;
        }
        /**
         * @brief 获取所有指定组件
         * @tparam T 组件类型
         * @return
        */
        template <typename T = Component>
        std::vector<T*> GetComponents() const
        {
            std::vector<T*> result{};
            std::deque<std::unique_ptr<Component>>& componentList = m_Components[typeid(T).hash_code()];
            for (auto& component : componentList)
            {
                result.push_back(static_cast<T*>(component.get()));
            }
            return result;
        }
        const std::map<UINT64, std::deque<std::unique_ptr<Component>>>& GetRawComponents() const
        {
            return m_Components;
        }
        /**
         * @brief 移除指定组件
         * @tparam T 组件类型
         * @param component 组件
         * @return
        */
        template <typename T = Component>
        bool RemoveComponent(const T* component)
        {
            if (m_Version == 0) return false;
            bool result = false;
            std::vector<std::unique_ptr<Component>>& componentList = m_Components[typeid(T).hash_code()];
            auto it = componentList.begin();
            for (; it != componentList.end(); ++it)
            {
                if (it->get() == component)
                {
                    componentList.erase(it);
                    result = true;
                    m_Version++;
                    break;
                }
            }
            return result;
        }
        template <typename T = Component>
        bool RemoveComponent()
        {
            if (m_Version == 0) return false;
            bool result = false;
            std::deque<std::unique_ptr<Component>>& componentList = m_Components[typeid(T).hash_code()];
            if (componentList.size() > 0)
            {
                m_Version++;
                componentList.pop_front();
            }
            return result;
        }

        void Destroy() { m_Version = 0; }

    private:
        bool m_Enabled{ true };
        std::unique_ptr<Transform> m_Transform{};
        std::map<UINT64, std::deque<std::unique_ptr<Component>>> m_Components{};

        UINT64 m_Version = 1;

    };
}