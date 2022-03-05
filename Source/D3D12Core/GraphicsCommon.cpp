﻿#include "pch.h"

#include "GraphicsCore.h"
#include "DescriptorHeap.h"
#include "GraphicsBuffer.h"

#include "GraphicsCommon.h"

using namespace std;
using namespace DirectX;


namespace Graphics
{
    D3D12_SAMPLER_DESC g_SamplerPointBorder; // 点采样边界色纹理
    D3D12_SAMPLER_DESC g_SamplerLinearBorder; // 线性采样边界色纹理
    D3D12_SAMPLER_DESC g_SamplerPointClamp; // 点采样钳位纹理
    D3D12_SAMPLER_DESC g_SamplerLinearClamp; // 线性采样钳位纹理
    D3D12_SAMPLER_DESC g_SamplerPointWarp; // 点采样平铺纹理
    D3D12_SAMPLER_DESC g_SamplerLinearWarp; // 线性采样平铺纹理
    D3D12_SAMPLER_DESC g_SamplerPointMirror; // 点采样镜像纹理
    D3D12_SAMPLER_DESC g_SamplerLinearMirror; // 线性采样镜像纹理


    void InitializeCommonSampler()
    {
        // 创建动态采样器
        D3D12_SAMPLER_DESC sampler{};
        sampler.MipLODBias = 0.0f;
        sampler.MaxAnisotropy = 1;
        sampler.BorderColor[0] = 0.0f;
        sampler.BorderColor[1] = 0.0f;
        sampler.BorderColor[2] = 0.0f;
        sampler.BorderColor[3] = 0.0f;
        sampler.MinLOD = 0;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;


        // SamplerPointBorder
        g_SamplerPointBorder = sampler;
        g_SamplerPointBorder.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        g_SamplerPointBorder.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        g_SamplerPointBorder.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        g_SamplerPointBorder.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        g_SamplerPointBorder.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        // SamplerLinearBorder
        g_SamplerLinearBorder = sampler;
        g_SamplerLinearBorder.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        g_SamplerLinearBorder.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        g_SamplerLinearBorder.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        g_SamplerLinearBorder.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        g_SamplerLinearBorder.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

        // SamplerPointClamp
        g_SamplerPointClamp = sampler;
        g_SamplerPointClamp.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        g_SamplerPointClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        g_SamplerPointClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        g_SamplerPointClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        g_SamplerPointClamp.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        // SamplerLinearClamp
        g_SamplerLinearClamp = sampler;
        g_SamplerLinearClamp.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        g_SamplerLinearClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        g_SamplerLinearClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        g_SamplerLinearClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        g_SamplerLinearClamp.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

        // SamplerPointWarp
        g_SamplerPointWarp = sampler;
        g_SamplerPointWarp.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        g_SamplerPointWarp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        g_SamplerPointWarp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        g_SamplerPointWarp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        g_SamplerPointWarp.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        // SamplerLinearWarp
        g_SamplerLinearWarp = sampler;
        g_SamplerLinearWarp.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        g_SamplerLinearWarp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        g_SamplerLinearWarp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        g_SamplerLinearWarp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        g_SamplerLinearWarp.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

        // SamplerPointMirror
        g_SamplerPointMirror = sampler;
        g_SamplerPointMirror.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        g_SamplerPointMirror.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        g_SamplerPointMirror.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        g_SamplerPointMirror.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        g_SamplerPointMirror.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        // SamplerLinearMirror
        g_SamplerLinearMirror = sampler;
        g_SamplerLinearMirror.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        g_SamplerLinearMirror.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        g_SamplerLinearMirror.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        g_SamplerLinearMirror.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        g_SamplerLinearMirror.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    }

}

