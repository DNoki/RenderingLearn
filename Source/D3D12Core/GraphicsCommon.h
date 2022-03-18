#pragma once

namespace Game
{
    class Mesh;
    class Shader;
    class Material;
}

namespace Resources
{
    class RenderTargetTexture;
    class DepthStencilTexture;
}

namespace Graphics
{
    class DescriptorHandle;

#pragma region 采样器

    extern DescriptorHandle g_SamplerPointBorder;   // 点采样边界色纹理
    extern DescriptorHandle g_SamplerLinearBorder;  // 线性采样边界色纹理
    extern DescriptorHandle g_SamplerPointClamp;    // 点采样钳位纹理
    extern DescriptorHandle g_SamplerLinearClamp;   // 线性采样钳位纹理
    extern DescriptorHandle g_SamplerPointWarp;     // 点采样平铺纹理
    extern DescriptorHandle g_SamplerLinearWarp;    // 线性采样平铺纹理
    extern DescriptorHandle g_SamplerPointMirror;   // 点采样镜像纹理
    extern DescriptorHandle g_SamplerLinearMirror;  // 线性采样镜像纹理

    extern DescriptorHandle g_SamplerLinearBorderCompare; // 阴影判断采样器

#pragma endregion

#pragma region 阴影贴图

    // 生成深度图着色器
    extern Game::Shader g_GenDepthShader;
    // 生成深度图材质
    extern Game::Material g_GenDepthMaterial;
    // 平行光阴影贴图
    extern Resources::DepthStencilTexture g_ShadowMapTexture;

#pragma endregion

#pragma region 延迟渲染

    // 深度贴图
    extern Resources::DepthStencilTexture g_DepthTexture;

#pragma endregion

#pragma region 后处理

    extern Game::Mesh g_BlitQuad;           // 位块传送用模型
    extern Game::Shader g_BlitShader;       // 位块传送着色器
    extern Game::Material g_BlitMaterial;   // 位块传送材质

    extern Resources::RenderTargetTexture g_RenderRtvTexture;
    extern Resources::DepthStencilTexture g_RenderDsvTexture;

#pragma endregion

    void InitializeCommonGraphicsResource();
}