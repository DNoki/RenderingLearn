#pragma once

#include "Component.h"
#include "ConstansBuffer.h"

namespace Game
{
    struct TransformBuffer
    {
        Matrix4x4 m_Model;
        Matrix4x4 m_IT_Model;
        Matrix4x4 m_MVP;
    };

    class Transform final : public Component
    {
    public:
        // 本地坐标位置
        Vector3 m_LocalPosition{ Vector3::Zero };
        // 本地坐标旋转
        Quaternion m_LocalRotation{ Quaternion::Identity };
        // 本地坐标缩放
        Vector3 m_LocalScale{ Vector3::One };

        Transform(GameObject& obj);
        virtual ~Transform() override = default;

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
        inline const ConstansBuffer<TransformBuffer>* GetTransformBuffer() const { return m_TransformBuffer.get(); }
        inline void RefleshTransformBuffer(const Matrix4x4& pv)
        {
            auto* mappingBuffer = m_TransformBuffer->GetMappingBuffer();
            mappingBuffer->m_Model = GetLocalToWorldMatrix();
            mappingBuffer->m_IT_Model = mappingBuffer->m_Model.Inverse();
            mappingBuffer->m_MVP = pv * mappingBuffer->m_Model;
        }

        /**
         * @brief 设置父对象
         * @param parent 父对象
         * @param worldPositionStays 保持世界空间位置
        */
        void SetParent(Transform* parent, bool worldPositionStays = true);
        inline Transform* GetParent() const { return m_Parent; }
        inline std::vector<Transform*>& GetChilds() { return m_Childs; }

        /**
         * @brief 获取本地到世界变换矩阵
         * @return
        */
        inline Matrix4x4 GetLocalToWorldMatrix() const
        {
            Matrix4x4 m;
            m.SetTRS(GetPosition(), GetEulerAngles(), GetLossyScale());
            return m;
        }
        /**
         * @brief 获取世界到本地变换矩阵
         * @return
        */
        inline Matrix4x4 GetWorldToLocalMatrix() const { return GetLocalToWorldMatrix().Inverse(); }

        /**
         * @brief 平移变换
         * @param offset 位移量
         * @param isWorld 是否以世界坐标为基准
        */
        inline void Translate(Vector3 offset, bool isWorld = false)
        {
            offset = isWorld ? offset : (GetRotation(true) * offset);
            SetPosition(offset + GetPosition(true), true);
        }
        /**
         * @brief 按轴角旋转变换
         * @param axis 旋转轴
         * @param angle 旋转角
         * @param isWorld 是否以世界坐标为基准
        */
        inline void Rotate(Vector3 axis, float angle, bool isWorld = false)
        {
            axis = isWorld ? axis : (GetRotation(true) * axis);
            SetRotation(Quaternion::CreateFromAxisAngle(axis, angle) * GetRotation(true), true);
        }
        /**
         * @brief 按欧拉角旋转变换
         * @param eulers 欧拉角
         * @param isWorld 是否以世界坐标为基准
        */
        inline void Rotate(Vector3 eulers, bool isWorld = false)
        {
            Rotate(Vector3::Up, eulers.y, isWorld);
            Rotate(Vector3::Right, eulers.x, isWorld);
            Rotate(Vector3::Forward, eulers.z, isWorld);
        }
        /**
         * @brief 按世界坐标中的位置和旋转轴角旋转变换（更改位置和旋转）
         * @param point 旋转中心
         * @param axis 旋转轴
         * @param angle 旋转角
        */
        void RotateAround(Vector3 point, Vector3 axis, float angle)
        {
            auto tempPos = GetPosition(true) - point;
            tempPos = Quaternion::CreateFromAxisAngle(axis, angle) * tempPos;
            SetPosition(tempPos + point, true);
            Rotate(axis, angle, true);
        }

        /**
         * @brief 旋转变换，使向前矢量指向 target 的当前位置。
         * @param target
         * @param up
        */
        inline void LookAt(Vector3 target, Vector3 up = Vector3::Up) { SetRotation(Quaternion::LookRotationLH(target - GetPosition(), up)); }

    private:
        // 父对象变换
        Transform* m_Parent{};
        std::vector<Transform*> m_Childs{};

        std::unique_ptr<ConstansBuffer<TransformBuffer>> m_TransformBuffer;

    };
}