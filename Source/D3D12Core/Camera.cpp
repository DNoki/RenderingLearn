#include "pch.h"

#include "GraphicsCore.h"
#include "Display.h"

#include "Camera.h"

using namespace Graphics;

namespace Game
{
    Camera::Camera() : m_Transform(),
        m_ProjectionMode(ProjectionMode::Perspective), m_FieldOfView(60.0f), m_OrthographicSize(5.0f),
        m_NearClipPlane(0.1f), m_FarClipPlane(1000.0f) {}


    Matrix4x4 Camera::GetProjectionMatrix() const
    {
        using namespace DirectX;
        Matrix4x4 projection{};
        switch (m_ProjectionMode)
        {
        case Game::ProjectionMode::Perspective:
        {
            ASSERT(0.1f < m_FieldOfView && m_FieldOfView < 179.0f);
            projection = XMMatrixPerspectiveFovLH(m_FieldOfView * Math::Deg2Rad, g_SwapChain.GetScreenAspect(), m_NearClipPlane, m_FarClipPlane);
        }
        break;
        case Game::ProjectionMode::Orthographic:
        {
            float orthSize = m_OrthographicSize * 2.0f;
            projection = XMMatrixOrthographicLH(orthSize * g_SwapChain.GetScreenAspect(), orthSize, m_NearClipPlane, m_FarClipPlane);
        }
        break;
        default: break;
        }
        return projection;
    }
}