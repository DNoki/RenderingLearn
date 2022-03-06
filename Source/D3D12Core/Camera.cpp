#include "pch.h"

#include "GraphicsCore.h"
#include "Display.h"
#include "Transform.h"

#include "Camera.h"

using namespace Graphics;

namespace Game
{
    Matrix4x4 Camera::GetProjectionMatrix() const
    {
        Matrix4x4 projection{};
        switch (m_ProjectionMode)
        {
        case Game::ProjectionMode::Perspective:
        {
            ASSERT(0.1f < m_FieldOfView && m_FieldOfView < 179.0f);
            projection = Matrix4x4::CreateFromPerspectiveLH(m_FieldOfView * Math::Deg2Rad, GraphicsManager::GetSwapChain()->GetScreenAspect(), m_NearClipPlane, m_FarClipPlane);
        }
        break;
        case Game::ProjectionMode::Orthographic:
        {
            float orthSize = m_OrthographicSize * 2.0f;
            projection = Matrix4x4::CreateFromOrthographicLH(orthSize * GraphicsManager::GetSwapChain()->GetScreenAspect(), orthSize, m_NearClipPlane, m_FarClipPlane);
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