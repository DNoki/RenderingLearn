#pragma once

namespace Game
{
    class Transform
    {
    public:
        // 本地坐标位置
        Vector3 LocalPosition;
        // 本地坐标旋转
        Vector3 LocalEulerAngles;
        // 本地坐标缩放
        Vector3 LocalScale;

        Transform() : LocalPosition(), LocalEulerAngles(), LocalScale(Vector3::One) {}

        // 获取变换矩阵
        inline Matrix4x4 GetLocalToWorldMatrix() const
        {
            Matrix4x4 m;
            m.SetTRS(LocalPosition, LocalEulerAngles, LocalScale);
            return m;
        }
        inline Matrix4x4 GetWorldToLocalMatrix() const { return GetLocalToWorldMatrix().Invert(); }
        inline Matrix4x4 GetViewMatrix() const
        {
            using namespace DirectX;
            Matrix4x4 m;
            Matrix4x4 t;
            Matrix4x4 r;
            auto pos = -LocalPosition;
            //pos.x *= -1.0f;
            //pos.y *= -1.0f;
            //pos.z *= -1.0f;
            //m.SetTRS(pos, LocalEulerAngles, LocalScale);

            t = XMMatrixTranslationFromVector(pos);

            r = XMMatrixRotationRollPitchYawFromVector(LocalEulerAngles);

            //m = t * r.Transpose();
            m = r.Transpose() * t;
            //m = m.Invert();

            return m;
        }

    private:

    };

    class Camera
    {
    public:
        Camera() = default;

    private:

    };
}