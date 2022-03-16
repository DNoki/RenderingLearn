#include "pch.h"

#include "GraphicsManager.h"
#include "DescriptorHeap.h"
#include "GraphicsBuffer.h"
#include "Mesh.h"
#include "PipelineState.h"
#include "Shader.h"
#include "Material.h"
#include "AppMain.h"

#include "GraphicsCommon.h"

using namespace std;
using namespace Game;


namespace Graphics
{
    DescriptorHandle g_SamplerPointBorder;   // 点采样边界色纹理
    DescriptorHandle g_SamplerLinearBorder;  // 线性采样边界色纹理
    DescriptorHandle g_SamplerPointClamp;    // 点采样钳位纹理
    DescriptorHandle g_SamplerLinearClamp;   // 线性采样钳位纹理
    DescriptorHandle g_SamplerPointWarp;     // 点采样平铺纹理
    DescriptorHandle g_SamplerLinearWarp;    // 线性采样平铺纹理
    DescriptorHandle g_SamplerPointMirror;   // 点采样镜像纹理
    DescriptorHandle g_SamplerLinearMirror;  // 线性采样镜像纹理

    Game::Mesh g_BlitQuad;
    Game::Shader g_BlitShader;
    Game::Material g_BlitMaterial;

    void InitializeCommonGraphicsResource()
    {
        // 初始化动态采样器
        {
            DescriptorHandle* const descriptorHandles[] =
            {
                &g_SamplerPointBorder,
                &g_SamplerLinearBorder,
                &g_SamplerPointClamp,
                &g_SamplerLinearClamp,
                &g_SamplerPointWarp,
                &g_SamplerLinearWarp,
                &g_SamplerPointMirror,
                &g_SamplerLinearMirror,
            };

            const D3D12_FILTER filters[] =
            {
                D3D12_FILTER_MIN_MAG_MIP_POINT,
                D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                D3D12_FILTER_MIN_MAG_MIP_POINT,
                D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                D3D12_FILTER_MIN_MAG_MIP_POINT,
                D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                D3D12_FILTER_MIN_MAG_MIP_POINT,
                D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            };

            const D3D12_TEXTURE_ADDRESS_MODE addressModes[] =
            {
                D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
            };

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
            sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

            for (int i = 0; i < _countof(descriptorHandles); i++)
            {
                *descriptorHandles[i] = DescriptorAllocator::Allocat(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

                sampler.Filter = filters[i];
                sampler.AddressU = addressModes[i];
                sampler.AddressV = addressModes[i];
                sampler.AddressW = addressModes[i];

                GraphicsManager::GetDevice()->CreateSampler(&sampler, *descriptorHandles[i]);
            }
        }

        // 位块传输用资源
        {
            g_BlitQuad = Mesh::CreateQuad(2.0f);
            g_BlitMaterial.SetName(L"BlitQuad");

            ShaderDesc shaderDesc{};
            shaderDesc.m_SemanticFlags = (1 << (int)VertexSemantic::Position) | (1 << (int)VertexSemantic::Texcoord);
            shaderDesc.m_ShaderFilePaths[static_cast<int>(ShaderType::VertexShader)] = Application::GetShaderPath().append("Blit_vs.cso");
            shaderDesc.m_ShaderFilePaths[static_cast<int>(ShaderType::PixelShader)] = Application::GetShaderPath().append("Blit_ps.cso");
            shaderDesc.m_CbvCount = 0;
            shaderDesc.m_SrvCount = 1;
            shaderDesc.m_SamplerCount = 1;
            g_BlitShader.Create(&shaderDesc);
            g_BlitShader.SetName(L"BlitShader");

            g_BlitMaterial.Create(&g_BlitShader);
            g_BlitMaterial.SetName(L"BlitMaterial");
            g_BlitMaterial.SetDepthEnable(false);

            g_BlitMaterial.BindSampler(0, g_SamplerPointClamp);
        }
    }

}

