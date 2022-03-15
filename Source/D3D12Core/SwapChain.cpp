#include "pch.h"

#include "AppMain.h"
#include "GraphicsManager.h"
#include "CommandQueue.h"
#include "RenderTexture.h"

#include "SwapChain.h"

// DirectX 图形基础结构 (DXGI) ：最佳实践 https://docs.microsoft.com/zh-cn/windows/win32/direct3darticles/dxgi-best-practices

using namespace std;
using namespace Resources;

namespace Graphics
{
    void SwapChain::CreateForHwnd(HWND hwnd, UINT count, DXGI_FORMAT format)
    {
        ASSERT(m_SwapChain == nullptr);

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
            m_SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // 指定交换链行为
        }

        // 全屏交换链描述，设置为 NULL 时将创建窗口交换链
        m_FullScreenDesc = nullptr;
        //m_FullScreenDesc->Windowed = TRUE;

        CHECK_HRESULT(GraphicsManager::GetFactory()->CreateSwapChainForHwnd(
            GraphicsManager::GetGraphicsCommandQueue()->GetD3D12CommandQueue(), // 对于 Direct3D 12，这是一个指向直接命令队列的指针
            hwnd,
            &m_SwapChainDesc,       // 交换链描述
            m_FullScreenDesc.get(), // 全屏交换链描述
            nullptr, // 限制内容的输出，为 NULL 时不限制
            (IDXGISwapChain1**)m_SwapChain.put()));
        SET_DEBUGNAME(m_SwapChain.get(), _T("SwapChain"));

        // 获取交换链描述
        m_SwapChain->GetDesc1(&m_SwapChainDesc);

        // 创建渲染目标描述符堆
        m_RtvDescriptorHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_SwapChainDesc.BufferCount);
        m_RtvDescriptorHeap.SetName(L"SwapChainRtv");
        m_DsvDescriptorHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
        m_DsvDescriptorHeap.SetName(L"SwapChainDsv");

        RebuildRenderTargets();

        // 设置交换链背景颜色。通常，除非交换链内容小于目标窗口，否则背景颜色是不可见的。
        //auto backColor = DXGI_RGBA{ 1.0f, 1.0f, 1.0f, 1.0f };
        //g_SwapChain->SetBackgroundColor(&backColor);
    }

    void SwapChain::Resize(UINT width, UINT height)
    {
        if (m_SwapChainDesc.Width == width && m_SwapChainDesc.Height == height)
            return;

        // TODO 需要在这里等待指令队列完成
        GraphicsManager::GetGraphicsCommandQueue()->WaitForQueueCompleted();

        m_SwapChainDesc.Width = width;
        m_SwapChainDesc.Height = height;

        m_RenderTargets.clear(); // 清除现有的渲染目标贴图

        // 重置交换链缓冲大小
        CHECK_HRESULT(m_SwapChain->ResizeBuffers(
            0, // 保留后台缓冲区数量
            m_SwapChainDesc.Width, m_SwapChainDesc.Height,
            DXGI_FORMAT_UNKNOWN, // 保留后台缓冲区格式
            0));

        // 获取交换链描述
        m_SwapChain->GetDesc1(&m_SwapChainDesc);

        RebuildRenderTargets();

        m_SwapChain->Present(1, 0); // 交换一次缓冲，使黑色填充屏幕

        // TODO 需要在这里等待指令队列完成
        GraphicsManager::GetGraphicsCommandQueue()->WaitForQueueCompleted();

        TRACE(L"Changing display resolution to %ux%u", m_SwapChainDesc.Width, m_SwapChainDesc.Height);
    }

    void SwapChain::RebuildRenderTargets()
    {
        m_RenderTargets.clear();
        m_RenderTargets.resize(m_SwapChainDesc.BufferCount);

        for (int i = 0; i < m_RenderTargets.size(); i++)
        {
            m_RenderTargets[i].reset(new RenderTexture());
            m_RenderTargets[i]->CreateRtvFromSwapChain(*this, i);
            m_RenderTargets[i]->SetName(Application::Format(L"SwapChain_RTV%d", i));
            m_RtvDescriptorHeap.BindRenderTargetView(i, *(m_RenderTargets[i]));
        }

        // 创建深度模板渲染贴图
        m_DepthStencils.reset(new RenderTexture());
        m_DepthStencils->PlacedCreate(RenderTextureType::DepthStencil, DXGI_FORMAT_D32_FLOAT, GetWidth(), GetHeight());
        m_DepthStencils->SetName(L"SwapChain_DSV");
        m_DsvDescriptorHeap.BindDepthStencilView(0, *m_DepthStencils);
    }
}
