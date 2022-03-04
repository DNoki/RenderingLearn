#pragma once

#include "Component.h"

namespace Game
{
    class Transform final : public Component
    {
    public:
        // 本地坐标位置
        Vector3 LocalPosition;
        // 本地坐标旋转
        Vector3 LocalEulerAngles; // TODO 使用四元数存储旋转
        // 本地坐标缩放
        Vector3 LocalScale;

        Transform(GameObject& obj);

        // 获取位置
        Vector3 GetPosition(bool isWorld = true) const;
        // 设置位置
        void SetPosition(const Vector3& pos, bool isWorld = true);
        // 获取旋转
        Quaternion GetRotation(bool isWorld = true) const;
        // 设置旋转
        void SetRotation(const Quaternion& rot, bool isWorld = true);
        // 获取欧拉角
        Vector3 GetEulerAngles(bool isWorld = true) const;
        // 设置欧拉角
        void SetEulerAngles(const Vector3& e, bool isWorld = true);
        // 获取全局比例
        Vector3 GetLossyScale() const;
        // 获取全局比例
        void SetLossyScale(const Vector3& s);

        // 获取前方向
        inline Vector3 GetForward() const { return GetRotation() * Vector3::Forward; }
        // 获取上方向
        inline Vector3 GetUp() const { return GetRotation() * Vector3::Up; }
        // 获取右方向
        inline Vector3 GetRight() const { return GetRotation() * Vector3::Right; }

        /**
         * @brief 设置父对象
         * @param parent 父对象
         * @param worldPositionStays 保持世界空间位置
        */
        void SetParent(Transform* parent, bool worldPositionStays = true);
        inline Transform* GetParent() const { return m_Parent; }
        inline std::vector<Transform*>& GetChilds() { return m_Childs; }

        // 获取本地到世界变换矩阵
        inline Matrix4x4 GetLocalToWorldMatrix() const
        {
            Matrix4x4 m;
            m.SetTRS(GetPosition(), GetEulerAngles(), GetLossyScale());
            return m;
        }
        // 获取世界到本地变换矩阵
        inline Matrix4x4 GetWorldToLocalMatrix() const { return GetLocalToWorldMatrix().Inverse(); }

        // 平移
        void Translate(Vector3 value, bool isWorld = false);
        // 旋转
        void Rotate(Vector3 axis, float angle, bool isWorld = false);
        void Rotate(Vector3 eulers, bool isWorld = false);
        void RotateAround(Vector3 point, Vector3 axis, float angle);

        // 旋转变换，使向前矢量指向 target 的当前位置。
        inline void LookAt(Vector3 target, Vector3 up = Vector3::Up) { SetRotation(Quaternion::LookRotationLH(target - GetPosition(), up)); }

    private:
        // 父对象变换
        Transform* m_Parent;
        std::vector<Transform*> m_Childs;

    };
}