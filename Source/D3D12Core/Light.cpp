#include "pch.h"

#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"

#include "Light.h"


namespace Game
{
    DirectionalLight::DirectionalLight(GameObject& obj) : Light(obj)
    {
        m_LightingBuffer.reset(new ConstansBuffer<DirectionalLightingBuffer>());
        m_LightingBuffer->PlacedCreate();
        m_LightingBuffer->SetName(Application::Format(L"%s (LightingBuffer)", obj.m_Name.c_str()));
    }

    Matrix4x4 DirectionalLight::GetLightSpaceMatrix(const Camera& camera) const
    {
        // 视图矩阵
        auto lightRotate = Matrix4x4::CreateFromRotation(this->GetTransform().GetRotation());
        auto lightPosition = Matrix4x4::CreateFromTranslation(camera.GetTransform().GetPosition()
            + camera.GetTransform().GetForward() * ShadowDistance * 0.5f
            + -this->GetTransform().GetForward() * ShadowDistance * 0.5f
        );
        Matrix4x4 lightView = (lightPosition * lightRotate).Inverse();
        //Matrix4x4 lightView = Matrix4x4::ZInverse * (lightPosition * lightRotate).Inverse();

        // 投影矩阵
        auto size = ShadowDistance * 0.5f;
        Matrix4x4 lightProjection = Matrix4x4::CreateFromOrthographicLH(size, size, ShadowNearPlaneOffset, 2 * ShadowDistance);

        return lightProjection * lightView;
    }

    void DirectionalLight::RefleshLightingBuffer()
    {
        auto* mappingBuffer = m_LightingBuffer->GetMappingBuffer();
        mappingBuffer->m_LightColor = m_LightColor;
        mappingBuffer->m_WorldSpaceLightPos = Vector4(GetTransform().GetForward(), 1.0f);
    }
}