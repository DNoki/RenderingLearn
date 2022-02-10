#include "pch.h"

#include "GraphicsCore.h"
#include "DescriptorHeap.h"

#include "GraphicsCommon.h"


namespace Graphics
{
    DescriptorHeap g_CommonSamplersDescriptorHeap;

    DescriptorHandle g_SamplerPointBorder; // 点采样边界色纹理
    DescriptorHandle g_SamplerLinearBorder; // 线性采样边界色纹理
    DescriptorHandle g_SamplerPointClamp; // 点采样钳位纹理
    DescriptorHandle g_SamplerLinearClamp; // 线性采样钳位纹理
    DescriptorHandle g_SamplerPointWarp; // 点采样平铺纹理
    DescriptorHandle g_SamplerLinearWarp; // 线性采样平铺纹理
    DescriptorHandle g_SamplerPointMirror; // 点采样镜像纹理
    DescriptorHandle g_SamplerLinearMirror; // 线性采样镜像纹理



    void InitializeCommonSampler()
    {
        // 创建动态采样器堆，绑定描述符
        g_CommonSamplersDescriptorHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 8);
        g_SamplerPointBorder = g_CommonSamplersDescriptorHeap.GetDescriptorHandle(0);
        g_SamplerLinearBorder = g_CommonSamplersDescriptorHeap.GetDescriptorHandle(1);
        g_SamplerPointClamp = g_CommonSamplersDescriptorHeap.GetDescriptorHandle(2);
        g_SamplerLinearClamp = g_CommonSamplersDescriptorHeap.GetDescriptorHandle(3);
        g_SamplerPointWarp = g_CommonSamplersDescriptorHeap.GetDescriptorHandle(4);
        g_SamplerLinearWarp = g_CommonSamplersDescriptorHeap.GetDescriptorHandle(5);
        g_SamplerPointMirror = g_CommonSamplersDescriptorHeap.GetDescriptorHandle(6);
        g_SamplerLinearMirror = g_CommonSamplersDescriptorHeap.GetDescriptorHandle(7);


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
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        g_Device->CreateSampler(&sampler, g_SamplerPointBorder);
        // SamplerLinearBorder
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        g_Device->CreateSampler(&sampler, g_SamplerLinearBorder);

        // SamplerPointClamp
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        g_Device->CreateSampler(&sampler, g_SamplerPointClamp);
        // SamplerLinearClamp
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        g_Device->CreateSampler(&sampler, g_SamplerLinearClamp);

        // SamplerPointWarp
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        g_Device->CreateSampler(&sampler, g_SamplerPointWarp);
        // SamplerLinearWarp
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        g_Device->CreateSampler(&sampler, g_SamplerLinearWarp);

        // SamplerPointMirror
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        g_Device->CreateSampler(&sampler, g_SamplerPointMirror);
        // SamplerLinearMirror
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        g_Device->CreateSampler(&sampler, g_SamplerLinearMirror);

    }
}

