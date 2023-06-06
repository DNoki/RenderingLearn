#include "pch.h"
#include "​GraphicsContext/SwapChain.h"

#include "​GraphicsResource/DescriptorAllocator.h"

//#include "AppMain.h"
//#include "GraphicsManager.h"
//#include "CommandQueue.h"
//#include "RenderTexture.h"


// DirectX 图形基础结构 (DXGI) ：最佳实践 https://docs.microsoft.com/zh-cn/windows/win32/direct3darticles/dxgi-best-practices

// 可变刷新率显示/关闭垂直同步 https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/variable-refresh-rate-displays

//using namespace std;
using namespace D3D12Core;

void SwapChainBuffer::Create(SwapChain& swapChain, UINT index)
{
    CHECK_HRESULT(swapChain.GetD3D12SwapChain()->GetBuffer(index, IID_PPV_ARGS(PutD3D12Resource())));

    m_ResourceDesc = m_Resource->GetDesc();
    m_ResourceStates = D3D12_RESOURCE_STATE_PRESENT;

    // 渲染呈现视图
    {
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = GetFormat();
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        m_RTV = DescriptorAllocator::Allocat(swapChain.GetGraphicsContext(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        swapChain.GetGraphicsContext()->GetDevice()->CreateRenderTargetView(m_Resource.get(), &rtvDesc, m_RTV);
    }

    // 着色器资源视图
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = GetFormat();
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        m_SRV = DescriptorAllocator::Allocat(swapChain.GetGraphicsContext(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        swapChain.GetGraphicsContext()->GetDevice()->CreateShaderResourceView(m_Resource.get(), &srvDesc, m_SRV);
    }
}

void SwapChain::CreateForHwnd(const GraphicsContext* graphicsContext, const CommandQueue* commandQueue, HWND hwnd, UINT count, DXGI_FORMAT format)
{
    ASSERT(m_SwapChain == nullptr);

    m_GraphicsContext = graphicsContext;

    // 计算窗口客户区大小
    //RECT clientRect;
    //GetClientRect(g_Hwnd, &clientRect);
    //g_DisplayWidth = clientRect.right - clientRect.left;
    //g_DisplayHeight = clientRect.bottom - clientRect.top;

    // 交换链描述
    {
        m_SwapChainDesc.Width = 0;     // 从窗口大小获取分辨率宽度
        m_SwapChainDesc.Height = 0;    // 从窗口大小获取分辨率高度
        m_SwapChainDesc.Format = format;                                // 显示格式
        m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // 后台缓冲区的表面使用情况和 CPU 访问选项
        m_SwapChainDesc.BufferCount = count;                           // 交换链中缓冲区数量
        m_SwapChainDesc.SampleDesc.Count = 1;                           // 多采样数量
        m_SwapChainDesc.SampleDesc.Quality = 0;                         // 多采样质量
        m_SwapChainDesc.Scaling = DXGI_SCALING_NONE;                    // 标识调整行为
        m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;     // 交换链使用的表示模型和用于在呈现表面后处理演示缓冲区内容的选项
        m_SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;             // 交换链后台缓冲区的透明度行为
        // 指定交换链行为 https://learn.microsoft.com/zh-cn/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_chain_flag
        m_SwapChainDesc.Flags =
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | // 允许调用 ResizeTarget 切换模式
            (m_VsyncEnable ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING); // 支持撕裂以启用可变刷新率
    }

    // 全屏交换链描述，设置为 NULL 时将创建窗口交换链
    m_FullScreenDesc = nullptr;
    //m_FullScreenDesc->Windowed = TRUE;

    CHECK_HRESULT(graphicsContext->GetFactory()->CreateSwapChainForHwnd(
        commandQueue->GetD3D12CommandQueue(), // 对于 Direct3D 12，这是一个指向直接命令队列的指针
        hwnd,
        &m_SwapChainDesc,       // 交换链描述
        m_FullScreenDesc.get(), // 全屏交换链描述
        nullptr, // 限制内容的输出，为 NULL 时不限制
        reinterpret_cast<IDXGISwapChain1**>(m_SwapChain.put())));
    GraphicsContext::SetDebugName(m_SwapChain.get(), TEXT("SwapChain"));

    // 获取交换链描述
    m_SwapChain->GetDesc1(&m_SwapChainDesc);

    RebuildRenderTargets();

    // 设置交换链背景颜色。通常，除非交换链内容小于目标窗口，否则背景颜色是不可见的。
    //auto backColor = DXGI_RGBA{ 1.0f, 1.0f, 1.0f, 1.0f };
    //g_SwapChain->SetBackgroundColor(&backColor);
}

void SwapChain::Resize(UINT width, UINT height)
{
    //if (m_SwapChainDesc.Width == width && m_SwapChainDesc.Height == height)
    //    return;

    //// TODO 需要在这里等待指令队列完成
    //GraphicsManager::GetGraphicsCommandQueue()->WaitForQueueCompleted();

    //m_SwapChainDesc.Width = width;
    //m_SwapChainDesc.Height = height;

    //m_RenderTargets.clear(); // 清除现有的渲染目标贴图

    //// 重置交换链缓冲大小
    //CHECK_HRESULT(m_SwapChain->ResizeBuffers(
    //    0, // 保留后台缓冲区数量
    //    m_SwapChainDesc.Width, m_SwapChainDesc.Height,
    //    DXGI_FORMAT_UNKNOWN, // 保留后台缓冲区格式
    //    VSYNC_ENABLE ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING));

    //// 获取交换链描述
    //m_SwapChain->GetDesc1(&m_SwapChainDesc);

    //RebuildRenderTargets();

    //m_SwapChain->Present(0, 0); // 交换一次缓冲，使黑色填充屏幕

    //TRACE(L"Changing display resolution to %ux%u", m_SwapChainDesc.Width, m_SwapChainDesc.Height);
}

void SwapChain::RebuildRenderTargets()
{
    m_RenderTargets.clear();
    m_RenderTargets.resize(m_SwapChainDesc.BufferCount);

    for (int i = 0; i < m_RenderTargets.size(); i++)
    {
        m_RenderTargets[i].reset(new SwapChainBuffer());
        //m_RenderTargets[i]->CreateFromSwapChain(*this, i);
        //m_RenderTargets[i]->SetName(Application::Format(L"SwapChain_RTV%d", i));

        //m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_RenderTargets[i]->PutD3D12Resource()));
        //m_RenderTargets[i]->GetResourceDesc()

        m_RenderTargets[i]->Create(*this, i);
    }


}
