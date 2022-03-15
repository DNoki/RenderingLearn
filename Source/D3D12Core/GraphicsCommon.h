#pragma once

namespace Game
{
    class Mesh;
    class Shader;
    class Material;
}

namespace Graphics
{
    inline D3D12_SAMPLER_DESC g_SamplerPointBorder;     // 点采样边界色纹理
    inline D3D12_SAMPLER_DESC g_SamplerLinearBorder;    // 线性采样边界色纹理
    inline D3D12_SAMPLER_DESC g_SamplerPointClamp;      // 点采样钳位纹理
    inline D3D12_SAMPLER_DESC g_SamplerLinearClamp;     // 线性采样钳位纹理
    inline D3D12_SAMPLER_DESC g_SamplerPointWarp;       // 点采样平铺纹理
    inline D3D12_SAMPLER_DESC g_SamplerLinearWarp;      // 线性采样平铺纹理
    inline D3D12_SAMPLER_DESC g_SamplerPointMirror;     // 点采样镜像纹理
    inline D3D12_SAMPLER_DESC g_SamplerLinearMirror;    // 线性采样镜像纹理

    extern Game::Mesh g_BlitQuad;
    extern Game::Shader g_BlitShader;
    extern Game::Material g_BlitMaterial;

    void InitializeCommonSampler();
}