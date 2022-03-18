#pragma once

#include "Component.h"

namespace Game
{
    class Camera;

    class Light : public Component
    {
    public:
        Light(GameObject& obj) : Component(obj) {}

    private:

    };

    class DirectionalLight : public Light
    {
    public:
        Vector4 m_LightColor = Vector4::One;
        float ShadowDistance = 40.0f;
        float ShadowNearPlaneOffset = 0.1f;

        DirectionalLight(GameObject& obj);

        Matrix4x4 GetLightViewMatrix() const;
        Matrix4x4 GetLightProjectionMatrix() const;

        void FillDirLightBuffer(ShaderCommon::DirLightBuffer* buffer);

    private:

    };
}