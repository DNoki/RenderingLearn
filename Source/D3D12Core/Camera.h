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
        inline Matrix4x4 GetTransformMatrix() const
        {
            //return Matrix4x4::CreateFromTransform(LocalPosition, LocalEulerAngles, LocalScale);
            return Matrix4x4();
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