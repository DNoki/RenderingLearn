#include "pch.h"
#include "​GraphicsContext/GraphicsContext.h"

using namespace D3D12Core;

GraphicsContext* GraphicsContext::CurrentInstance = nullptr;

void ::GraphicsContext::Initialize()
{
    CurrentInstance = this;

    UINT nDXGIFactoryFlags = 0U;
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
        winrt::com_ptr<IDXGIAdapter4> pAdapter;
        winrt::com_ptr<ID3D12Device6> pDevice;
        SIZE_T MaxSize = 0;
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(adapterIndex, reinterpret_cast<IDXGIAdapter1**>(pAdapter.put())); adapterIndex++)
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
                    TRACE(L"D3D12-capable hardware found:  %s (%u MB)\n", desc.Description, desc.DedicatedVideoMemory >> 20);
                    MaxSize = desc.DedicatedVideoMemory;
                    m_Adapter = std::move(pAdapter);
                }
            }
            // 重新设定智能指针
            pAdapter = nullptr;
        }
        if (MaxSize > 0)
            m_Device = std::move(pDevice);
        ASSERT(m_Device != nullptr, L"ERROR::D3D12设备对象创建失败");

        DXGI_QUERY_VIDEO_MEMORY_INFO GpuMemoryInfo{}; // TODO 描述当前的GPU内存预算参数
        CHECK_HRESULT(m_Adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &GpuMemoryInfo));
        {
            m_Adapter->GetDesc3(&m_AdapterDesc);
        }

        UINT nodeCount = m_Device->GetNodeCount(); // 查询物理适配器数目
        ASSERT(nodeCount > 0);

        SetDebugName(m_Adapter.get(), TEXT("IDXGIAdapter"));
        SetDebugName(m_Device.get(), TEXT("Device"));
    }
}

void D3D12Core::GraphicsContext::Destroy()
{
    m_Device = nullptr;
    m_Adapter = nullptr;
    m_Factory = nullptr;
    m_D3D12Debug = nullptr;
}

void D3D12Core::GraphicsContext::SetAltEnterEnable(HWND winHandle, bool enable)
{
    // 启用或防止 DXGI 响应 Alt + Enter
    CHECK_HRESULT(m_Factory->MakeWindowAssociation(winHandle, enable ? 0 : DXGI_MWA_NO_ALT_ENTER));
}

void D3D12Core::GraphicsContext::SetDebugName(IDXGIObject* pObj, const String& name)
{
    static UINT DebugIndex = 0;

    if (!pObj) return;

    String indexedName = Format(TEXT("%s_%06d"), name.c_str(), DebugIndex++);
    CHECK_HRESULT(pObj->SetPrivateData(WKPDID_D3DDebugObjectNameW, static_cast<UINT>((indexedName.size() + 1) * sizeof(wchar_t)), indexedName.c_str()));
}

void D3D12Core::GraphicsContext::SetDebugName(ID3D12Object* pObj, const String& name)
{
    static UINT DebugIndex = 0;

    if (!pObj) return;

    String indexedName = Format(_T("%s_%06d"), name.c_str(), DebugIndex++);
    CHECK_HRESULT(pObj->SetName(indexedName.c_str()));
}
