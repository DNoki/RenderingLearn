#include "pch.h"

#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"
#include "RenderTexture.h"

#include "Light.h"


namespace Game
{
    DirectionalLight::DirectionalLight(GameObject& obj) : Light(obj)
    {
    }

    Matrix4x4 DirectionalLight::GetLightViewMatrix() const
    {
        auto pos = GetTransform().GetPosition() - (GetTransform().GetForward() * ShadowDistance * 0.5f);
        auto rot = GetTransform().GetRotation();

        // 返回世界到光照空间的矩阵
        auto t = Matrix4x4::CreateFromTranslation(-pos);
        auto r = Matrix4x4::CreateFromRotation(rot);

        return r.Transpose() * t;
    }
    Matrix4x4 DirectionalLight::GetLightProjectionMatrix() const
    {
        // 投影矩阵
        auto size = 10.0f;
        auto projection = Matrix4x4::CreateFromOrthographicLH(size, size, ShadowNearPlaneOffset, ShadowDistance);

        // 计算 ReversedZ
        projection._33 = -projection._33;
        projection._43 = 1.0f - projection._43;

        return projection;
    }
    void DirectionalLight::FillDirLightBuffer(ShaderCommon::DirLightBuffer* buffer, const Resources::DepthStencilTexture* dsv)
    {
        buffer->_DirLight_Color = m_LightColor;
        buffer->_DirLight_WorldPos = Vector4(GetTransform().GetForward(), 1.0f);
        buffer->_DirLight_WorldToLight = GetLightViewMatrix();
        buffer->_DirLight_LightToClip = GetLightProjectionMatrix();
        buffer->_DirLight_WorldToLightClip = buffer->_DirLight_LightToClip * buffer->_DirLight_WorldToLight;

        buffer->_DirLight_ShadowmapParams.x = static_cast<float>(dsv->GetWidth());
        buffer->_DirLight_ShadowmapParams.y = static_cast<float>(dsv->GetHeight());
        buffer->_DirLight_ShadowmapParams.z = 1.0f / buffer->_DirLight_ShadowmapParams.x;
        buffer->_DirLight_ShadowmapParams.w = 1.0f / buffer->_DirLight_ShadowmapParams.y;
    }
}