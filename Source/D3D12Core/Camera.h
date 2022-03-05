#pragma once

#include "Component.h"

namespace Game
{
    /**
     * @brief 投影模式
    */
    enum class ProjectionMode
    {
        Perspective = 0,    // 透视
        Orthographic = 1,   // 正交
    };

    class Camera final : public Component
    {
    public:
        ProjectionMode m_ProjectionMode; // 投影模式
        float m_FieldOfView; // 透视模式的视角 角度制
        float m_OrthographicSize; // 正交模式下相机的半尺寸
        float m_NearClipPlane;  // 近裁面
        float m_FarClipPlane;   // 远裁面

        Camera(GameObject& obj);
        virtual ~Camera() override = default;


        Matrix4x4 GetProjectionMatrix() const;
        Matrix4x4 GetViewMatrix() const;

    private:
    };
}