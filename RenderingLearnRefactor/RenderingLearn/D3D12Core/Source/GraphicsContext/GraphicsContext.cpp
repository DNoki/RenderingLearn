#include "pch.h"
#include "​GraphicsContext/GraphicsContext.h"

using namespace D3D12Core;

void ::GraphicsContext::Initialize()
{
    CurrentInstance = this;

    uint32 nDXGIFactoryFlags = 0U;
#if DEBUG
    // 启用调试层（需要图形工具“可选功能”）。
    // 注意：在设备创建后启用调试层将使活动设备无效。
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(m_D3D12Debug.put()))))
    {
        m_D3D12Debug->EnableDebugLayer();
        nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif // DEBUG

    // 创建 DirectX 图形基础设施 (DXGI) 
    {
        CHECK_HRESULT(CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(m_Factory.put())));
        GraphicsContext::SetDebugName(m_Factory.get(), TEXT("IDXGIFactory"));
    }

    // 创建D3D12设备对象接口
    {
        ComPtr<IDXGIAdapter4> pAdapter;
        ComPtr<ID3D12Device6> pDevice;
        SIZE_T MaxSize = 0;
        for (uint32 adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(adapterIndex, reinterpret_cast<IDXGIAdapter1**>(pAdapter.put())); adapterIndex++)
        {
            // 遍历所有适配器
            DXGI_ADAPTER_DESC3 desc;
            pAdapter->GetDesc3(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {} // 软件虚拟适配器，跳过
            else if (desc.DedicatedVideoMemory > MaxSize) // 选择最大专用适配器内存（显存）
            {
                auto hr = D3D12CreateDevice(pAdapter.get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(pDevice.put()));
                if (SUCCEEDED(hr))
                {
                    TRACE(TEXT("D3D12-capable hardware found:  %s (%u MB)\n"), desc.Description, desc.DedicatedVideoMemory >> 20);
                    MaxSize = desc.DedicatedVideoMemory;
                    m_Adapter = std::move(pAdapter);
                }
            }
            // 重新设定智能指针
            pAdapter = nullptr;
        }
        if (MaxSize > 0)
            m_Device = std::move(pDevice);
        ASSERT(m_Device != nullptr, TEXT("ERROR::D3D12设备对象创建失败"));
        SetDebugName(m_Adapter.get(), TEXT("GraphicsAdapter"));
        SetDebugName(m_Device.get(), TEXT("GraphicsDevice"));

        // 获取显卡信息
        const auto GpuCount = GetGpuCount();
        ASSERT(GpuCount > 0);
        for (uint32 i = 0; i < GpuCount; ++i)
        {
            m_AdapterDesc.push_back(DXGI_ADAPTER_DESC3());
            CHECK_HRESULT(m_Adapter->GetDesc3(&m_AdapterDesc.back()));
        }
    }
}

void GraphicsContext::Destroy()
{
    m_Device = nullptr;
    m_Adapter = nullptr;
    m_Factory = nullptr;
    m_D3D12Debug = nullptr;
}

uint32 GraphicsContext::GetGpuCount() const
{
    return  m_Device->GetNodeCount();
}
String GraphicsContext::GetGpuDescription(uint32 Node) const
{
    return m_AdapterDesc[Node].Description;
}

uint64 GraphicsContext::GetGpuDedicatedMemory(uint32 Node) const
{
    return m_AdapterDesc[Node].DedicatedVideoMemory;
}

uint64 GraphicsContext::GetGpuSharedMemory(uint32 Node) const
{
    return m_AdapterDesc[Node].SharedSystemMemory;
}

auto GraphicsContext::GetGpuMemoryBudget(uint32 Node) const -> uint64
{
    DXGI_QUERY_VIDEO_MEMORY_INFO GpuMemoryInfo{};
    CHECK_HRESULT(m_Adapter->QueryVideoMemoryInfo(Node, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &GpuMemoryInfo));
    return GpuMemoryInfo.Budget;
}

uint64 GraphicsContext::GetGpuUsagedMemory(uint32 Node) const
{
    DXGI_QUERY_VIDEO_MEMORY_INFO GpuMemoryInfo{};
    CHECK_HRESULT(m_Adapter->QueryVideoMemoryInfo(Node, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &GpuMemoryInfo));
    return GpuMemoryInfo.CurrentUsage;
}

void GraphicsContext::SetAltEnterEnable(HWND winHandle, bool enable)
{
    // 启用或防止 DXGI 响应 Alt + Enter
    CHECK_HRESULT(m_Factory->MakeWindowAssociation(winHandle, enable ? 0 : DXGI_MWA_NO_ALT_ENTER));
}

void GraphicsContext::SetDebugName(IDXGIObject* pObj, const String& name)
{
    static uint32 DebugIndex = 0;

    if (!pObj) return;

    String indexedName = Format(TEXT("%s_%06d"), name.c_str(), DebugIndex++);
    CHECK_HRESULT(pObj->SetPrivateData(WKPDID_D3DDebugObjectNameW, static_cast<uint32>((indexedName.size() + 1) * sizeof(TCHAR)), indexedName.c_str()));
}

void GraphicsContext::SetDebugName(ID3D12Object* pObj, const String& name)
{
    static uint32 DebugIndex = 0;

    if (!pObj) return;

    String indexedName = Format(_T("%s_%06d"), name.c_str(), DebugIndex++);
    CHECK_HRESULT(pObj->SetName(indexedName.c_str()));
}
