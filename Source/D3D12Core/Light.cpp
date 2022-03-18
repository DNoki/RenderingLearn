#include "pch.h"

#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"

#include "Light.h"


namespace Game
{
    DirectionalLight::DirectionalLight(GameObject& obj) : Light(obj)
    {
        m_LightingBuffer.reset(new ConstansBuffer<ShaderCommon::DirLightBuffer>());
        m_LightingBuffer->PlacedCreate();
        m_LightingBuffer->SetName(Application::Format(L"%s (LightingBuffer)", obj.m_Name.c_str()));
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

    void DirectionalLight::RefleshLightingBuffer()
    {
        auto* mappingBuffer = m_LightingBuffer->GetMappingBuffer();
        mappingBuffer->_DirLight_Color = m_LightColor;
        mappingBuffer->_DirLight_WorldPos = Vector4(GetTransform().GetForward(), 1.0f);
        mappingBuffer->_DirLight_WorldToLight = GetLightViewMatrix();
        mappingBuffer->_DirLight_LightToClip = GetLightProjectionMatrix();
        mappingBuffer->_DirLight_WorldToLightClip = mappingBuffer->_DirLight_LightToClip * mappingBuffer->_DirLight_WorldToLight;
    }
}