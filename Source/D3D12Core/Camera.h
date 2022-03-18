#pragma once

#include "Component.h"
#include "ConstansBuffer.h"

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
        ProjectionMode m_ProjectionMode{ ProjectionMode::Perspective }; // 投影模式
        float m_FieldOfView{ 60.0f }; // 透视模式的视角 角度制
        float m_OrthographicSize{ 5.0f }; // 正交模式下相机的半尺寸
        float m_NearClipPlane{ 0.1f };  // 近裁面
        float m_FarClipPlane{ 1000.0f };   // 远裁面

        Camera(GameObject& obj);
        virtual ~Camera() override = default;


        Matrix4x4 GetProjectionMatrix() const;
        Matrix4x4 GetViewMatrix() const;

        inline const ConstansBuffer<ShaderCommon::CameraBuffer>* GetCameraBuffer() const { return m_CameraBuffer.get(); }
        inline void RefleshCameraBuffer()
        {
            m_CameraBuffer->GetMappingBuffer()->_Camera_Project = GetProjectionMatrix();
            m_CameraBuffer->GetMappingBuffer()->_Camera_View = GetViewMatrix();
            m_CameraBuffer->GetMappingBuffer()->_Camera_I_VP = (m_CameraBuffer->GetMappingBuffer()->_Camera_Project * m_CameraBuffer->GetMappingBuffer()->_Camera_View).Inverse();
        }

    private:
        std::unique_ptr<ConstansBuffer<ShaderCommon::CameraBuffer>> m_CameraBuffer;

    };
}