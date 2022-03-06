#pragma once

namespace Game
{
    class GameObject;
    class Transform;

    /**
     * @brief 组件
    */
    class Component
    {
    public:
        virtual ~Component() { OnDestroy(); }

        /**
         * @brief 获取游戏对象
         * @return 
        */
        inline GameObject& GetGameObject() const { return m_GameObject; }
        /**
         * @brief 获取变换
         * @return 
        */
        inline Transform& GetTransform() const { return m_Transform; }

        virtual void OnDestroy() {}

    protected:
        Component(GameObject& obj);

    private:
        GameObject& m_GameObject;
        Transform& m_Transform;

    };
}