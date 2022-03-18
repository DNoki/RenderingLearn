#include "pch.h"

#include "GraphicsManager.h"
#include "SwapChain.h"
#include "Transform.h"
#include "GameObject.h"

#include "Camera.h"

using namespace Graphics;

namespace Game
{
    Camera::Camera(GameObject& obj) : Component(obj)
    {
        m_CameraBuffer.reset(new ConstansBuffer<ShaderCommon::CameraBuffer>());
        m_CameraBuffer->PlacedCreate();
        m_CameraBuffer->SetName(Application::Format(L"%s (CameraBuffer)", obj.m_Name.c_str()));
    }

    Matrix4x4 Camera::GetProjectionMatrix() const
    {
        // 使用 反向-Z 提高深度精度 http://project-asura.com/blog/archives/5638
        // ①使近裁面为 1，远裁面为 0
        // ②深度比较函数使用 Greater
        // ③深度清空值为 0.0
        // 
        // 实际上 reversedZ * projection
        //static const Matrix4x4 reversedZ = Matrix4x4(
        //    1.0f, 0.0f, 0.0f, 0.0f,
        //    0.0f, 1.0f, 0.0f, 0.0f,
        //    0.0f, 0.0f, -1.0f, 0.0f,
        //    0.0f, 0.0f, 1.0f, 1.0f);

        Matrix4x4 projection{};
        switch (m_ProjectionMode)
        {
        case Game::ProjectionMode::Perspective:
        {
            ASSERT(0.1f < m_FieldOfView && m_FieldOfView < 179.0f);
            projection = Matrix4x4::CreateFromPerspectiveLH(m_FieldOfView * Math::Deg2Rad, GraphicsManager::GetSwapChain()->GetScreenAspect(), m_NearClipPlane, m_FarClipPlane);

            // 计算 ReversedZ
            //projection._33 = 1.0f - projection._33;
            //projection._43 = -projection._43;

            // 计算 ReversedZ，对远裁面取最大值并计算极限
            projection._33 = 0.0f;
            projection._43 = m_NearClipPlane;
        }
        break;
        case Game::ProjectionMode::Orthographic:
        {
            float orthSize = m_OrthographicSize * 2.0f;
            projection = Matrix4x4::CreateFromOrthographicLH(orthSize * GraphicsManager::GetSwapChain()->GetScreenAspect(), orthSize, m_NearClipPlane, m_FarClipPlane);

            // 计算 ReversedZ
            projection._33 = -projection._33;
            projection._43 = 1.0f - projection._43;
        }
        break;
        default: break;
        }
        return projection;
    }
    Matrix4x4 Camera::GetViewMatrix() const
    {
        auto& transform = GetTransform();

        // 返回世界到相机的矩阵
        auto t = Matrix4x4::CreateFromTranslation(-GetTransform().GetPosition());
        auto r = Matrix4x4::CreateFromRotation(GetTransform().GetRotation());

        return r.Transpose() * t;
    }
}