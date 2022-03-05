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

        inline GameObject& operator = (const GameObject & obj) = delete;
        inline GameObject& operator = (GameObject && obj) = default;

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

        /**
         * @brief 添加组件
         * @tparam T
         * @return
        */
        template <class T = Component>
        T& AddComponent()
        {
            auto component = make_unique<T>(*this);

            auto result = component.get();
            m_Components.push_back(move(component));

            //OnAddComponent(result);
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
            T* tryGet = nullptr;
            for (unsigned int i = 0; i < m_Components.size(); i++)
            {
                tryGet = static_cast<T*>(m_Components[i].get());
                if (tryGet) break;
            }
            return tryGet;
        }
        /**
         * @brief 获取所有指定组件
         * @tparam T 组件类型
         * @return
        */
        template <typename T = Component>
        std::vector<T*> GetComponents() const
        {
            auto result = std::vector<T*>();
            for (unsigned int i = 0; i < m_Components.size(); i++)
            {
                T* tryGet = static_cast<T*>(m_Components[i].get());
                if (tryGet) result.push_back(tryGet);
            }
            return result;
        }

    private:
        bool m_Enabled;
        std::unique_ptr<Transform> m_Transform;
        std::vector<std::unique_ptr<Component>> m_Components;

    };
}