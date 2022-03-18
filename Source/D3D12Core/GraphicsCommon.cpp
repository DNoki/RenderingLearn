#include "pch.h"

#include "GraphicsManager.h"
#include "DescriptorHeap.h"
#include "GraphicsBuffer.h"
#include "Mesh.h"
#include "PipelineState.h"
#include "Shader.h"
#include "Material.h"
#include "RenderTexture.h"
#include "AppMain.h"

#include "GraphicsCommon.h"

using namespace std;
using namespace Game;
using namespace Resources;


namespace Graphics
{
#pragma region 采样器

    DescriptorHandle g_SamplerPointBorder;   // 点采样边界色纹理
    DescriptorHandle g_SamplerLinearBorder;  // 线性采样边界色纹理
    DescriptorHandle g_SamplerPointClamp;    // 点采样钳位纹理
    DescriptorHandle g_SamplerLinearClamp;   // 线性采样钳位纹理
    DescriptorHandle g_SamplerPointWarp;     // 点采样平铺纹理
    DescriptorHandle g_SamplerLinearWarp;    // 线性采样平铺纹理
    DescriptorHandle g_SamplerPointMirror;   // 点采样镜像纹理
    DescriptorHandle g_SamplerLinearMirror;  // 线性采样镜像纹理

    DescriptorHandle g_SamplerLinearBorderCompare; // 阴影判断采样器

#pragma endregion

#pragma region 阴影贴图

    Shader g_GenDepthShader;
    Material g_GenDepthMaterial;
    DepthStencilTexture g_ShadowMapTexture;

#pragma endregion

#pragma region 延迟渲染

    DepthStencilTexture g_DepthTexture;

#pragma endregion

#pragma region 后处理

    Mesh g_BlitQuad;
    Shader g_BlitShader;
    Material g_BlitMaterial;

    RenderTargetTexture g_RenderRtvTexture;
    DepthStencilTexture g_RenderDsvTexture;

#pragma endregion

    void InitializeCommonGraphicsResource()
    {
        UINT defalutShadowMapSize = 1024;
        UINT defalutRenderSizeX = 1920, defalutRenderSizeY = 1080;

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

            {
                g_SamplerLinearBorderCompare = DescriptorAllocator::Allocat(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

                sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
                sampler.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
                sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
                sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
                sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;

                GraphicsManager::GetDevice()->CreateSampler(&sampler, g_SamplerLinearBorderCompare);
            }
        }

        // 位块传输用资源
        {
            g_BlitQuad = Mesh::CreateQuad(2.0f);
            g_BlitQuad.SetName(L"BlitQuad");

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

        // 平行光阴影用资源
        {
            ShaderDesc shaderDesc{};
            shaderDesc.m_SemanticFlags = (1 << (int)VertexSemantic::Position);
            shaderDesc.m_ShaderFilePaths[static_cast<int>(ShaderType::VertexShader)] = Application::GetShaderPath().append("GenerateDepth_vs.cso");
            shaderDesc.m_ShaderFilePaths[static_cast<int>(ShaderType::PixelShader)] = Application::GetShaderPath().append("GenerateDepth_ps.cso");
            shaderDesc.m_CbvCount = 2;
            shaderDesc.m_SrvCount = 0;
            shaderDesc.m_SamplerCount = 0;
            g_GenDepthShader.Create(&shaderDesc);
            g_GenDepthShader.SetName(L"GenDirLightShadowMapShader");

            g_GenDepthMaterial.Create(&g_GenDepthShader);
            g_GenDepthMaterial.SetName(L"GenDirLightShadowMapMaterial");
            g_GenDepthMaterial.SetDepthBias(-8000);// 阴影贴图与深度图实际写入值适当减小(Reverse-Z)

            // 阴影贴图
            g_ShadowMapTexture.PlacedCreate(DXGI_FORMAT_D32_FLOAT, defalutShadowMapSize, defalutShadowMapSize);
            g_ShadowMapTexture.SetName(L"ShadowMap");
        }

        // 延迟渲染共通资源
        {
            g_DepthTexture.PlacedCreate(DXGI_FORMAT_D32_FLOAT, defalutRenderSizeX, defalutRenderSizeY);
            g_DepthTexture.SetName(L"场景深度");
        }

        // 后处理共通资源
        {
            // 渲染目标呈现贴图
            g_RenderRtvTexture.PlacedCreate(DXGI_FORMAT_R8G8B8A8_UNORM, defalutRenderSizeX, defalutRenderSizeY, Color(0.0f, 0.2f, 0.4f, 1.0f));
            g_RenderRtvTexture.SetName(L"FinalRtvTexture");
            // 渲染目标深度贴图
            g_RenderDsvTexture.PlacedCreate(DXGI_FORMAT_D32_FLOAT, defalutRenderSizeX, defalutRenderSizeY);
            g_RenderDsvTexture.SetName(L"FinalDsvTexture");
        }
    }

}

