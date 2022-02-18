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

        // 获取前方向
        Vector3 GetForward() const;
        // 获取上方向
        Vector3 GetUp() const;
        // 获取右方向
        Vector3 GetRight() const;

        // 获取本地到世界变换矩阵
        inline Matrix4x4 GetLocalToWorldMatrix() const
        {
            Matrix4x4 m;
            m.SetTRS(LocalPosition, LocalEulerAngles, LocalScale);
            return m;
        }
        // 获取世界到本地变换矩阵
        inline Matrix4x4 GetWorldToLocalMatrix() const { return GetLocalToWorldMatrix().Invert(); }

        // 平移
        void Translate(Vector3 value, bool isWorld = false);
        // 旋转
        void Rotate(Vector3 axis, float angle, bool isWorld = false);
        void Rotate(Vector3 eulers, bool isWorld = false);
        void RotateAround(Vector3 point, Vector3 axis, float angle);

        // 旋转变换，使向前矢量指向 target 的当前位置。
        void LookAt(Vector3 target, Vector3 up = Vector3::Up);

    private:

    };

    /**
     * @brief 投影模式
    */
    enum class ProjectionMode
    {
        Perspective = 0,    // 透视
        Orthographic = 1,   // 正交
    };

    class Camera
    {
    public:
        Transform m_Transform;
        ProjectionMode m_ProjectionMode; // 投影模式
        float m_FieldOfView; // 透视模式的视角 角度制
        float m_OrthographicSize; // 正交模式下相机的半尺寸
        float m_NearClipPlane;  // 近裁面
        float m_FarClipPlane;   // 远裁面

        Camera();


        Matrix4x4 GetProjectionMatrix() const;
        inline Matrix4x4 GetViewMatrix() const
        {
            // 返回世界到相机的矩阵
            auto t = Matrix4x4::CreateFromTranslation(-m_Transform.LocalPosition);
            auto r = Matrix4x4::CreateFromRotation(m_Transform.LocalEulerAngles);
#ifdef USE_COLUMN_MAJOR
            return r.Transpose() * t;
#else
            return t * r.Transpose();
#endif // USE_COLUMN_MAJOR
        }

    private:
    };
}