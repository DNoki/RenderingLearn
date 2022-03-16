#include "pch.h"

#include "GraphicsMemory.h"
#include "GraphicsManager.h"
#include "SwapChain.h"

#include "RenderTexture.h"

using namespace std;
using namespace Graphics;

namespace Resources
{
#if 0
    void RenderTexture::DirectCreate(RenderTextureType type, DXGI_FORMAT format, UINT64 width, UINT height, const D3D12_CLEAR_VALUE* clearValue)
    {
        m_Type = type;

        // 设置清除优化值
        {
            if (clearValue)
                m_ClearValue = *clearValue;
            else
            {
                switch (m_Type)
                {
                case Resources::RenderTextureType::RenderTarget:
                    m_ClearValue.Color[0] = 0.0f;
                    m_ClearValue.Color[1] = 0.0f;
                    m_ClearValue.Color[2] = 0.0f;
                    m_ClearValue.Color[3] = 1.0f;
                    break;
                case Resources::RenderTextureType::DepthStencil:
                    m_ClearValue.DepthStencil.Depth = 0.0f;
                    m_ClearValue.DepthStencil.Stencil = 0;
                    break;
                default: break;
                }
            }
            m_ClearValue.Format = format;
        }

        // 资源描述
        {
            m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
            m_ResourceDesc.Flags = type == RenderTextureType::RenderTarget ?
                D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET :
                D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        }

        // 资源状态
        m_ResourceStates = type == RenderTextureType::RenderTarget ?
            (D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE) :
            D3D12_RESOURCE_STATE_DEPTH_WRITE;

        auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        CHECK_HRESULT(GraphicsManager::GetDevice()->CreateCommittedResource(
            &heapType,
            D3D12_HEAP_FLAG_NONE,
            &m_ResourceDesc,
            m_ResourceStates, // 允许深度模板缓冲写入
            &m_ClearValue,
            IID_PPV_ARGS(PutD3D12Resource())));

        if (type == RenderTextureType::RenderTarget)
        {
            // 渲染呈现视图描述
            m_RtvDesc.reset(new D3D12_RENDER_TARGET_VIEW_DESC());
            m_RtvDesc->Format = format;
            m_RtvDesc->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        }
        else
        {
            // 深度模板视图描述
            m_DsvDesc.reset(new D3D12_DEPTH_STENCIL_VIEW_DESC());
            m_DsvDesc->Format = format;
            m_DsvDesc->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            m_DsvDesc->Flags = D3D12_DSV_FLAG_NONE;
        }
    }

    void RenderTexture::PlacedCreate(RenderTextureType type, DXGI_FORMAT format, UINT64 width, UINT height, const D3D12_CLEAR_VALUE* clearValue)
    {
        m_Type = type;

        // 设置清除优化值
        {
            if (clearValue)
                m_ClearValue = *clearValue;
            else
            {
                switch (m_Type)
                {
                case Resources::RenderTextureType::RenderTarget:
                    m_ClearValue.Color[0] = 0.0f;
                    m_ClearValue.Color[1] = 0.0f;
                    m_ClearValue.Color[2] = 0.0f;
                    m_ClearValue.Color[3] = 1.0f;
                    break;
                case Resources::RenderTextureType::DepthStencil:
                    m_ClearValue.DepthStencil.Depth = 0.0f;
                    m_ClearValue.DepthStencil.Stencil = 0;
                    break;
                default: break;
                }
            }
            m_ClearValue.Format = format;
        }

        // 资源描述
        {
            m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
            m_ResourceDesc.Flags = type == RenderTextureType::RenderTarget ?
                D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET :
                D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        }

        // 资源状态
        m_ResourceStates = type == RenderTextureType::RenderTarget ?
            (D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE) :
            D3D12_RESOURCE_STATE_DEPTH_WRITE;

        m_PlacedResourceDesc.m_HeapType = D3D12_HEAP_TYPE_DEFAULT;
        m_PlacedResourceDesc.m_OptimizedClearValue = &m_ClearValue;

        GraphicsMemory::PlacedResource(*this);

        if (type == RenderTextureType::RenderTarget)
        {
            // 渲染呈现视图描述
            m_RtvDesc.reset(new D3D12_RENDER_TARGET_VIEW_DESC());
            m_RtvDesc->Format = format;
            m_RtvDesc->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        }
        else
        {
            // 深度模板视图描述
            m_DsvDesc.reset(new D3D12_DEPTH_STENCIL_VIEW_DESC());
            m_DsvDesc->Format = format;
            m_DsvDesc->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            m_DsvDesc->Flags = D3D12_DSV_FLAG_NONE;
        }
    }

#if 0
    void RenderTexture::DirectCreateRTV(DXGI_FORMAT format, UINT64 width, UINT height, Color clearColor)
    {
        // 清除优化值
        {
            m_ClearValue.Format = format;
            CopyMemory(m_ClearValue.Color, clearColor, sizeof(Color));
        }

        // 资源描述
        {
            m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);

            m_ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        }

        // 资源状态
        //m_ResourceStates = D3D12_RESOURCE_STATE_RENDER_TARGET;
        m_ResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        CHECK_HRESULT(GraphicsManager::GetDevice()->CreateCommittedResource(
            &heapType,
            D3D12_HEAP_FLAG_NONE,
            &m_ResourceDesc,
            m_ResourceStates,
            &m_ClearValue,
            IID_PPV_ARGS(PutD3D12Resource())));

        // 渲染呈现视图描述
        m_RtvDesc.reset(new D3D12_RENDER_TARGET_VIEW_DESC());
        m_RtvDesc->Format = format;
        m_RtvDesc->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    }

    void RenderTexture::DirectCreateDSV(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth, UINT8 optStencil)
    {
        // 清除优化值
        {
            m_ClearValue.Format = format;
            m_ClearValue.DepthStencil.Depth = optDepth;
            m_ClearValue.DepthStencil.Stencil = optStencil;
        }

        // 资源描述
        {
            m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);

            // 允许为资源创建深度模板视图，并允许资源转换到 D3D12_RESOURCE_STATE_DEPTH_WRITE 和/或 D3D12_RESOURCE_STATE_DEPTH_READ 状态。
            m_ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        }

        // 资源状态
        m_ResourceStates = D3D12_RESOURCE_STATE_DEPTH_WRITE; // 允许深度模板缓冲写入

        auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        CHECK_HRESULT(GraphicsManager::GetDevice()->CreateCommittedResource(
            &heapType,
            D3D12_HEAP_FLAG_NONE,
            &m_ResourceDesc,
            m_ResourceStates, // 允许深度模板缓冲写入
            &m_ClearValue,
            IID_PPV_ARGS(PutD3D12Resource())));
        SET_DEBUGNAME(m_Resource.get(), _T("RenderTexture"));

        // 深度模板视图描述
        m_DsvDesc.reset(new D3D12_DEPTH_STENCIL_VIEW_DESC());
        m_DsvDesc->Format = format;
        m_DsvDesc->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        m_DsvDesc->Flags = D3D12_DSV_FLAG_NONE;
    }
#endif

    void RenderTexture::CreateRtvFromSwapChain(const SwapChain& swapChain, UINT index)
    {
        ASSERT(m_Resource == nullptr);

        m_RtvDesc = nullptr;
        CHECK_HRESULT(swapChain.GetD3D12SwapChain()->GetBuffer(index, IID_PPV_ARGS(PutD3D12Resource())));
        SET_DEBUGNAME(m_Resource.get(), _T("RenderTexture"));

        m_ResourceDesc = m_Resource->GetDesc();
        m_ResourceStates = D3D12_RESOURCE_STATE_PRESENT;
    }
#endif // 0

    void RenderTargetTexture::DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, Color optColor)
    {
        InitDesc(format, width, height, optColor);

        // 创建资源
        auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        CHECK_HRESULT(GraphicsManager::GetDevice()->CreateCommittedResource(
            &heapType,
            D3D12_HEAP_FLAG_NONE,
            &m_ResourceDesc,
            m_ResourceStates,
            &m_ClearValue,
            IID_PPV_ARGS(PutD3D12Resource())));
    }

    void RenderTargetTexture::PlacedCreate(DXGI_FORMAT format, UINT64 width, UINT height, Color optColor)
    {
        InitDesc(format, width, height, optColor);

        m_PlacedResourceDesc.m_HeapType = D3D12_HEAP_TYPE_DEFAULT;
        m_PlacedResourceDesc.m_OptimizedClearValue = &m_ClearValue;

        GraphicsMemory::PlacedResource(*this);
    }
    void RenderTargetTexture::CreateFromSwapChain(const Graphics::SwapChain& swapChain, UINT index)
    {
        ASSERT(m_Resource == nullptr);

        m_RtvDesc = nullptr;
        CHECK_HRESULT(swapChain.GetD3D12SwapChain()->GetBuffer(index, IID_PPV_ARGS(PutD3D12Resource())));

        m_ResourceDesc = m_Resource->GetDesc();
        m_ResourceStates = D3D12_RESOURCE_STATE_PRESENT;
    }
    void RenderTargetTexture::InitDesc(DXGI_FORMAT format, UINT64 width, UINT height, Color optColor)
    {
        ASSERT(m_Resource == nullptr);

        // 设置清除优化值
        {
            m_ClearValue.Format = format;
            CopyMemory(m_ClearValue.Color, &optColor, sizeof(Color));
        }

        // 资源描述
        {
            m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
            m_ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        }

        // 资源状态
        m_ResourceStates = D3D12_RESOURCE_STATE_RENDER_TARGET;

        // 渲染呈现视图描述
        m_RtvDesc.reset(new D3D12_RENDER_TARGET_VIEW_DESC());
        m_RtvDesc->Format = format;
        m_RtvDesc->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    }


    void DepthStencilTexture::DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth, UINT8 optStencil)
    {
        InitDesc(format, width, height, optDepth, optStencil);

        auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        CHECK_HRESULT(GraphicsManager::GetDevice()->CreateCommittedResource(
            &heapType,
            D3D12_HEAP_FLAG_NONE,
            &m_ResourceDesc,
            m_ResourceStates,
            &m_ClearValue,
            IID_PPV_ARGS(PutD3D12Resource())));
    }

    void DepthStencilTexture::PlacedCreate(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth, UINT8 optStencil)
    {
        InitDesc(format, width, height, optDepth, optStencil);

        m_PlacedResourceDesc.m_HeapType = D3D12_HEAP_TYPE_DEFAULT;
        m_PlacedResourceDesc.m_OptimizedClearValue = &m_ClearValue;

        GraphicsMemory::PlacedResource(*this);
    }

    void DepthStencilTexture::InitDesc(DXGI_FORMAT format, UINT64 width, UINT height, float optDepth, UINT8 optStencil)
    {
        ASSERT(m_Resource == nullptr);

        // 设置清除优化值
        {
            m_ClearValue.Format = format;
            m_ClearValue.DepthStencil.Depth = optDepth;
            m_ClearValue.DepthStencil.Stencil = optStencil;
        }

        // 资源描述
        {
            m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
            m_ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        }

        // 资源状态
        m_ResourceStates = D3D12_RESOURCE_STATE_DEPTH_WRITE;

        // 深度模板视图描述
        m_DsvDesc.reset(new D3D12_DEPTH_STENCIL_VIEW_DESC());
        m_DsvDesc->Format = format;
        m_DsvDesc->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        m_DsvDesc->Flags = D3D12_DSV_FLAG_NONE;
    }

}
