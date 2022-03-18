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

    extern DescriptorHandle g_SamplerPointBorder;   // 点采样边界色纹理
    extern DescriptorHandle g_SamplerLinearBorder;  // 线性采样边界色纹理
    extern DescriptorHandle g_SamplerPointClamp;    // 点采样钳位纹理
    extern DescriptorHandle g_SamplerLinearClamp;   // 线性采样钳位纹理
    extern DescriptorHandle g_SamplerPointWarp;     // 点采样平铺纹理
    extern DescriptorHandle g_SamplerLinearWarp;    // 线性采样平铺纹理
    extern DescriptorHandle g_SamplerPointMirror;   // 点采样镜像纹理
    extern DescriptorHandle g_SamplerLinearMirror;  // 线性采样镜像纹理

    extern DescriptorHandle g_SamplerLinearBorderCompare;

    extern Game::Mesh g_BlitQuad;
    extern Game::Shader g_BlitShader;
    extern Game::Material g_BlitMaterial;

    extern Game::Shader g_GenDirLightShadowMapShader;
    extern Game::Material g_GenDirLightShadowMapMaterial;

    extern Resources::DepthStencilTexture g_ShadowMapTexture;

    extern Resources::RenderTargetTexture g_RenderRtvTexture;
    extern Resources::DepthStencilTexture g_RenderDsvTexture;

    void InitializeCommonGraphicsResource();
}