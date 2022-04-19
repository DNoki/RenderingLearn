
#ifdef CONSOLE

#define CONSTANT_TYPE struct
#define CONSTANT_TYPE_REGISTER(r)

#define float4x4 Matrix4x4
#define float4 Vector4

#elif SHADER_COMPILE

#define CONSTANT_TYPE cbuffer
#define CONSTANT_TYPE_REGISTER(r) : register(r)

#endif // CONSOLE


#ifdef CONSOLE
namespace ShaderCommon {
#endif // CONSOLE


    CONSTANT_TYPE CameraBuffer CONSTANT_TYPE_REGISTER(b0)
    {
        float4x4 _Camera_Project;
        float4x4 _Camera_View;
        float4x4 _Camera_I_VP;
        float4 _Camera_WorldPos;
        float4 _Camera_ScreenParams;
    };


    CONSTANT_TYPE ModelBuffer CONSTANT_TYPE_REGISTER(b1)
    {
        float4x4 _Model_LocalToWorld;
        float4x4 _Model_IT_M;
        float4x4 _Model_MVP;
    };


    CONSTANT_TYPE DirLightBuffer CONSTANT_TYPE_REGISTER(b2)
    {
        float4 _DirLight_Color;
        float4 _DirLight_WorldPos;
        float4x4 _DirLight_WorldToLight;
        float4x4 _DirLight_LightToClip;
        float4x4 _DirLight_WorldToLightClip;
        float4 _DirLight_ShadowmapParams;
    };


    // Reverse-Z 的无限远裁面深度值转到视角空间
    float ReverseInfDepthToViewDepth(float depth, float nearClip)
    {
        return nearClip / depth;
    }
    // Reverse-Z 的深度值转到线性空间
    float ReverseDepthToLinearDepth(float depth, float nearClip, float farClip)
    {
        return nearClip / (depth * (farClip - nearClip) + nearClip);
    }
    // Reverse-Z 的深度值转到视角空间
    float ReverseDepthToViewDepth(float depth, float nearClip, float farClip)
    {
        return farClip * ReverseDepthToLinearDepth(depth, nearClip, farClip);
    }


#ifdef CONSOLE
}
#endif // CONSOLE