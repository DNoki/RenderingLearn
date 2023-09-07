#include "pch.h"
#include "Resource/RenderTexture.h"

using namespace D3D12Core;
using namespace D3D12Viewer;

#if false
void RenderTexture::DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, Color optColor)
{
    InitDesc(format, width, height, optColor);

    // 创建资源
    auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CHECK_HRESULT(GraphicsManager::GetInstance().GetDevice()->CreateCommittedResource(
        &heapType,
        D3D12_HEAP_FLAG_NONE,
        &m_ResourceDesc,
        m_ResourceStates,
        &m_ClearValue,
        IID_PPV_ARGS(PutD3D12Resource())));

    InitDescriptor();
}
#endif

void RenderTexture::Create(D3D12Core::RenderTargetType type, DXGI_FORMAT format, UINT64 width, UINT height, D3D12_CLEAR_VALUE clearValue)
{
    m_Type = type;
    //InitDesc(format, width, height, optColor);



    m_PlacedResourceDesc.m_HeapType = D3D12_HEAP_TYPE_DEFAULT;
    m_PlacedResourceDesc.m_OptimizedClearValue = &m_ClearValue;

    GraphicsMemory::PlacedResource(*this);

    InitDescriptor();
}

void RenderTexture::InitDesc(DXGI_FORMAT format, UINT64 width, UINT height, Color optColor)
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

void RenderTexture::InitDescriptor()
{
    // 渲染呈现视图
    {
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = GetFormat();
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        m_RtvOrDsv = DescriptorAllocator::Allocat(*GraphicsContext::GetCurrentInstance(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        GraphicsManager::GetInstance().GetDevice()->CreateRenderTargetView(m_Resource.get(), &rtvDesc, m_RtvOrDsv);
    }

    // 着色器资源视图
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = GetFormat();
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        m_SRV = DescriptorAllocator::Allocat(*GraphicsContext::GetCurrentInstance(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        GraphicsManager::GetInstance().GetDevice()->CreateShaderResourceView(m_Resource.get(), &srvDesc, m_SRV);
    }
}
