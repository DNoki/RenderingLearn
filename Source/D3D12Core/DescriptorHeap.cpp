#include "pch.h"

#include "GraphicsResource.h"
#include "RenderTexture.h"
#include "GraphicsManager.h"

#include "DescriptorHeap.h"

using namespace std;
using namespace winrt;

namespace Graphics
{
    // --------------------------------------------------------------------------
    /*
        描述符堆
        https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/descriptor-heaps-overview

        描述符
        https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/descriptors-overview


        描述符堆包含不属于管道状态对象（PSO）的许多对象类型。包括：
            着色器可见描述符：
                ・着色器资源视图（SRV）
                ・无序访问视图（UAV）
                ・常量缓冲区视图（CBV）
                ・取样器（Sampler）
                    创建取样器不需要实际分配内存（不需要创建 ID3D12Resource）
            非着色器可见描述符：
                ・呈现目标视图（RTV）
                ・深度模板视图（DSV）

        SRV、UAV、CBV 可以保存在同一个描述符堆中

        描述符堆的使用
            在任何时候最多可以绑定一个 CBV/SRV/UAV 组合描述符堆和一个取样器堆

    */
    // --------------------------------------------------------------------------


    void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count, bool isShaderVisible)
    {
        ASSERT(m_DescriptorHeap == nullptr);
        m_DescriptorHeapDesc = {};

        // 是否为着色器可见（SRV、UAV、CBV、Sampler）
        isShaderVisible = (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) && isShaderVisible;

        // 描述符堆描述
        m_DescriptorHeapDesc.NumDescriptors = count;
        m_DescriptorHeapDesc.Type = type;
        m_DescriptorHeapDesc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        m_DescriptorHeapDesc.NodeMask = NODEMASK;


        // 创建描述符堆
        CHECK_HRESULT(GraphicsManager::GetDevice()->CreateDescriptorHeap(&m_DescriptorHeapDesc, IID_PPV_ARGS(m_DescriptorHeap.put())));

        // 单个描述符大小
        m_DescriptorSize = GraphicsManager::GetDevice()->GetDescriptorHandleIncrementSize(type);

        // 描述符起始句柄
        m_StartDescriptorHandle = {
            m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            isShaderVisible ? m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN }
        };
    }

    DescriptorHandle DescriptorHeap::GetDescriptorHandle(UINT index) const
    {
        ASSERT(0 <= index || index < GetDescriptorsCount());
        return m_StartDescriptorHandle + index * m_DescriptorSize;
    }

#if 0
    void DescriptorHeap::BindConstantBufferView(int index, const IBufferResource& buffer) const
    {
        ASSERT(GetHeapType() == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
        cbvDesc.BufferLocation = buffer.GetGpuVirtualAddress();
        // 常量缓冲视图要求对齐到 256
        cbvDesc.SizeInBytes = UINT_UPPER(buffer.GetBufferSize(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

        ASSERT(buffer.GetBufferSize() == cbvDesc.SizeInBytes, L"WARNING::常量缓冲资源大小未对齐到256。");

        GraphicsManager::GetDevice()->CreateConstantBufferView(&cbvDesc, GetDescriptorHandle(index));
    }

    void DescriptorHeap::BindShaderResourceView(int index, const Texture& tex) const
    {
        ASSERT(GetHeapType() == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = tex.GetResourceDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        GraphicsManager::GetDevice()->CreateShaderResourceView(tex.GetD3D12Resource(), &srvDesc, GetDescriptorHandle(index));
    }

    void DescriptorHeap::BindSampler(int index, const D3D12_SAMPLER_DESC& samplerDesc) const
    {
        ASSERT(GetHeapType() == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        GraphicsManager::GetDevice()->CreateSampler(&samplerDesc, GetDescriptorHandle(index));
    }

    void DescriptorHeap::BindRenderTargetView(int index, const Resources::RenderTargetTexture& renderTex) const
    {
        ASSERT(GetHeapType() == D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        GraphicsManager::GetDevice()->CreateRenderTargetView(renderTex.GetD3D12Resource(), renderTex.GetRtvDesc(), GetDescriptorHandle(index));
    }
    void DescriptorHeap::BindDepthStencilView(int index, const Resources::DepthStencilTexture& renderTex) const
    {
        ASSERT(GetHeapType() == D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        GraphicsManager::GetDevice()->CreateDepthStencilView(renderTex.GetD3D12Resource(), renderTex.GetDsvDesc(), GetDescriptorHandle(index));
    }
#endif


    struct AllocatorHeap
    {
    public:
        unique_ptr<DescriptorHeap> m_DescriptorHeap{};
        UINT m_NextHandle{};
    };

    class DescriptorAllocatorImpl
    {
    public:
        DescriptorAllocatorImpl() = default;

        DescriptorHandle Allocat(D3D12_DESCRIPTOR_HEAP_TYPE type)
        {
            if (m_CurrentHeap[type] == nullptr || (m_CurrentHeap[type]->m_NextHandle >= m_CurrentHeap[type]->m_DescriptorHeap->GetDescriptorsCount()))
            {
                m_Heaps[type].push_back(AllocatorHeap());
                m_CurrentHeap[type] = &m_Heaps[type].back();
                m_CurrentHeap[type]->m_DescriptorHeap.reset(new DescriptorHeap());
                m_CurrentHeap[type]->m_DescriptorHeap->Create(type, 256, false);

                static const wstring heapNames[] =
                {
                    L"CBV_SRV_UAV",
                    L"Sampler",
                    L"RTV",
                    L"DSV",
                };
                m_CurrentHeap[type]->m_DescriptorHeap->SetName(heapNames[type].c_str());
            }
            return m_CurrentHeap[type]->m_DescriptorHeap->GetDescriptorHandle(m_CurrentHeap[type]->m_NextHandle++);
        }

    private:
        vector<AllocatorHeap> m_Heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{};

        AllocatorHeap* m_CurrentHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{};

    } g_DescriptorAllocatorImpl;

    DescriptorHandle DescriptorAllocator::Allocat(D3D12_DESCRIPTOR_HEAP_TYPE type)
    {
        return g_DescriptorAllocatorImpl.Allocat(type);
    }
}
