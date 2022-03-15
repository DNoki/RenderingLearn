#pragma once

#include "Component.h"
#include "ConstansBuffer.h"

namespace Game
{
    class Camera;

    class Light : public Component
    {
    public:
        Light(GameObject& obj) : Component(obj) {}

    private:

    };

    struct DirectionalLightingBuffer
    {
        Vector4 m_LightColor;
        Vector4 m_WorldSpaceLightPos;
    };

    class DirectionalLight : public Light
    {
    public:
        Vector4 m_LightColor = Vector4::One;
        float ShadowDistance = 40.0f;
        float ShadowNearPlaneOffset = 0.1f;

        DirectionalLight(GameObject& obj);

        Matrix4x4 GetLightSpaceMatrix(const Camera& camera) const;

        inline const ConstansBuffer<DirectionalLightingBuffer>* GetLightingBuffer() const { return m_LightingBuffer.get(); }
        void RefleshLightingBuffer();


    private:
        std::unique_ptr<ConstansBuffer<DirectionalLightingBuffer>> m_LightingBuffer;

    };
}