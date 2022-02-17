#pragma once

namespace Game
{
    class Transform
    {
    public:
        // 本地坐标位置
        Vector3 LocalPosition;
        // 本地坐标旋转
        Vector3 LocalEulerAngles; // TODO 使用四元数存储旋转
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
            // 返回世界到相机的矩阵
            auto t = Matrix4x4::CreateFromTranslation(-LocalPosition);
            auto r = Matrix4x4::CreateFromRotation(LocalEulerAngles);
#ifdef USE_COLUMN_MAJOR
            return r.Transpose() * t;
#else
            return t * r.Transpose();
#endif // USE_COLUMN_MAJOR
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