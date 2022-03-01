﻿#include "pch.h"

#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "RenderTexture.h"
#include "GraphicsCommon.h"

#include "PipelineState.h"

#include "Display.h"
#include "CommandQueue.h"
#include "CommandList.h"

#include "GameTime.h"
#include "SampleResource.h"
#include "AppMain.h"

#include "GraphicsCore.h"


using namespace winrt;
using namespace std;
using namespace Application;
using namespace Game;


namespace Graphics
{
    wstring g_TitleFormat = L"%s  GPU(%s)  FPS:%.2f";
    wstring g_TitleGPU;

    GraphicsManager GraphicsManager::m_GraphicsManager = GraphicsManager();

    CommandList g_GraphicsCommandList;

    MultiRenderTargets g_CurrentRenderTargets;


    void InitializeCommonSampler();

    void Initialize()
    {
        // --------------------------------------------------------------------------
        // 创建指令列表
        g_GraphicsCommandList.Create(D3D12_COMMAND_LIST_TYPE_DIRECT, true);

        // 重置命令列表以其便能够帮助初始化资源复制命令
        //g_GraphicsCommandList.Reset(nullptr);


        // --------------------------------------------------------------------------
        // 初始化共通动态采样器
        InitializeCommonSampler();


        // --------------------------------------------------------------------------
        // 创建示例PSO
        InitShader();


        // --------------------------------------------------------------------------
        // 创建示例资源
        InitMesh();
        InitTexture2D();

        // --------------------------------------------------------------------------
        InitCommandListBundle();

        // --------------------------------------------------------------------------
        // 由于初始化贴图时需要执行复制命令
        // 执行命令列表
        GraphicsManager::GetGraphicsCommandQueue()->ExecuteCommandLists(&g_GraphicsCommandList);


        // 等待命令列表执行。 我们在主循环中重用相同的命令列表，但现在，我们只想等待设置完成后再继续。
        GraphicsManager::GetGraphicsCommandQueue()->WaitForQueueCompleted();
    }


    void OnRender()
    {
        auto& g_GraphicsCommandQueue = *GraphicsManager::GetGraphicsCommandQueue();
        PIXSetMarker(g_GraphicsCommandQueue.GetD3D12CommandQueue(), PIX_COLOR(255, 0, 0), L"渲染 Marker");
        PIXBeginEvent(g_GraphicsCommandQueue.GetD3D12CommandQueue(), PIX_COLOR(0, 255, 0), L"渲染 Event");

        auto& g_SwapChain = *GraphicsManager::GetSwapChain();

        // --------------------------------------------------------------------------
        // 渲染
        // 填充命令列表
        {
            // 获取当前后台缓冲索引
            auto cbbi = g_SwapChain.GetCurrentBackBufferIndex();
            g_CurrentRenderTargets = MultiRenderTargets{};
            g_CurrentRenderTargets.SetRenderTarget(0, g_SwapChain.GetRtvDescHandle(cbbi), g_SwapChain.GetRenderTarget(cbbi).GetFormat());
            g_CurrentRenderTargets.SetDepthStencil(g_SwapChain.GetDsvDescHandle(), g_SwapChain.GetDepthStencil().GetFormat());

            // 重置命令列表
            g_GraphicsCommandList.Reset();

            // 设置必要的状态。
            auto viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(g_SwapChain.GetWidth()), static_cast<float>(g_SwapChain.GetHeight()));
            g_GraphicsCommandList.RSSetViewports(1, &viewport);
            auto scissorRect = CD3DX12_RECT(0, 0, g_SwapChain.GetWidth(), g_SwapChain.GetHeight());
            g_GraphicsCommandList.RSSetScissorRects(1, &scissorRect);

            // 指示后台缓冲区将用作渲染目标。
            auto& currentRenderTarget = g_SwapChain.GetRenderTarget(cbbi);
            auto barriers1 = CD3DX12_RESOURCE_BARRIER::Transition(currentRenderTarget.GetD3D12Resource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            g_GraphicsCommandList.ResourceBarrier(1, &barriers1);

            // 设置渲染目标
            //g_GraphicsCommandList.OMSetRenderTargets(1, g_SwapChain.GetRtvDescHandle(cbbi), FALSE, g_SwapChain.GetDsvDescHandle()); // TODO 实现多目标渲染
            g_GraphicsCommandList.OMSetRenderTargets(&g_CurrentRenderTargets);

            // 清除渲染目标贴图
            const Color clearColor = Color(0.0f, 0.2f, 0.4f, 1.0f);
            g_GraphicsCommandList.ClearRenderTargetView(g_SwapChain.GetRtvDescHandle(cbbi), clearColor, 0, nullptr);
            g_GraphicsCommandList.ClearDepthStencilView(g_SwapChain.GetDsvDescHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

            SampleDraw();

            // 指示现在将使用后台缓冲区来呈现。
            auto barriers2 = CD3DX12_RESOURCE_BARRIER::Transition(currentRenderTarget.GetD3D12Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            g_GraphicsCommandList.ResourceBarrier(1, &barriers2);
        }
        // 执行命令列表
        g_GraphicsCommandQueue.ExecuteCommandLists(&g_GraphicsCommandList);

        PIXEndEvent(g_GraphicsCommandQueue.GetD3D12CommandQueue());

        // 呈现帧。
        CHECK_HRESULT(g_SwapChain.GetD3D12SwapChain()->Present(1, 0));

        g_GraphicsCommandQueue.WaitForQueueCompleted();

        TimeSystem::AddFrameCompleted();
        TimeSystem::AddSwapFrameCompleted();

        SetWindowTitle(Format(g_TitleFormat.c_str(), WINDOW_TITLE, g_TitleGPU.c_str(), Time::GetFPS()));
    }

    class GraphicsManagerImpl
    {
    public:
#if DEBUG
        winrt::com_ptr<ID3D12Debug3> m_D3D12Debug;
#endif // DEBUG
        winrt::com_ptr<IDXGIFactory7> m_Factory;
        winrt::com_ptr<IDXGIAdapter4> m_Adapter;
        winrt::com_ptr<ID3D12Device6> m_Device;

        std::unique_ptr<CommandQueue> m_GraphicsCommandQueue;   // 图形命令队列
        std::unique_ptr<CommandQueue> m_ComputeCommandQueue;    // 计算命令队列
        std::unique_ptr<CommandQueue> m_CopyCommandQueue;       // 拷贝命令队列

        std::unique_ptr<SwapChain> m_SwapChain;

        GraphicsManagerImpl() = default;
        inline void Initialize()
        {
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
                CHECK_HRESULT(m_Factory->MakeWindowAssociation(Application::g_Hwnd, DXGI_MWA_NO_ALT_ENTER)); // 防止 DXGI 响应 Alt+Enter
                SET_DEBUGNAME(m_Factory.get(), _T("IDXGIFactory"));
            }

            // 创建D3D12设备对象接口
            {
                com_ptr<IDXGIAdapter4> pAdapter;
                com_ptr<ID3D12Device6> pDevice;
                SIZE_T MaxSize = 0;
                for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(adapterIndex, (IDXGIAdapter1**)(pAdapter.put())); adapterIndex++)
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
                            m_Adapter = move(pAdapter);
                        }
                    }
                    // 重新设定智能指针
                    pAdapter = nullptr;
                }
                if (MaxSize > 0)
                    m_Device = move(pDevice);
                ASSERT(m_Device != nullptr, L"ERROR::D3D12设备对象创建失败");

                DXGI_QUERY_VIDEO_MEMORY_INFO GpuMemoryInfo{}; // TODO 描述当前的GPU内存预算参数
                CHECK_HRESULT(m_Adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &GpuMemoryInfo));
                {
                    auto title = Application::GetWindowTitle();
                    DXGI_ADAPTER_DESC3 desc;
                    m_Adapter->GetDesc3(&desc);
                    g_TitleGPU = desc.Description;
                }

                UINT nodeCount = m_Device->GetNodeCount(); // 查询物理适配器数目
                ASSERT(nodeCount > 0);

                SET_DEBUGNAME(m_Adapter.get(), _T("IDXGIAdapter"));
                SET_DEBUGNAME(m_Device.get(), _T("Device"));
            }

            // 创建图形命令队列
            {
                m_GraphicsCommandQueue.reset(new CommandQueue());
                m_ComputeCommandQueue.reset(new CommandQueue());
                m_CopyCommandQueue.reset(new CommandQueue());
                m_GraphicsCommandQueue->Create(D3D12_COMMAND_LIST_TYPE_DIRECT);
                m_ComputeCommandQueue->Create(D3D12_COMMAND_LIST_TYPE_COMPUTE);
                m_CopyCommandQueue->Create(D3D12_COMMAND_LIST_TYPE_COPY);
            }

            // 初始化交换链
            m_SwapChain.reset(new SwapChain());
            m_SwapChain->CreateForHwnd(Application::g_Hwnd, SWAP_FRAME_BACK_BUFFER_COUNT, SWAP_CHAIN_RENDER_TARGET_FORMAT);
        }
        inline void Destory()
        {
            // 等待队列指令完成后关闭
            m_GraphicsCommandQueue->CloseQueue();
            m_ComputeCommandQueue->CloseQueue();
            m_CopyCommandQueue->CloseQueue();

            m_SwapChain = nullptr;

            m_GraphicsCommandQueue = nullptr;
            m_ComputeCommandQueue = nullptr;
            m_CopyCommandQueue = nullptr;

            m_Device = nullptr;
            m_Adapter = nullptr;
            m_Factory = nullptr;

#if DEBUG
            m_D3D12Debug = nullptr;
#endif // DEBUG
        }

    private:

    } g_GraphicsManager;

    IDXGIFactory7* GraphicsManager::GetFactory()
    {
        return g_GraphicsManager.m_Factory.get();
    }

    ID3D12Device6* GraphicsManager::GetDevice()
    {
        return g_GraphicsManager.m_Device.get();
    }

    CommandQueue* GraphicsManager::GetGraphicsCommandQueue()
    {
        return g_GraphicsManager.m_GraphicsCommandQueue.get();
    }

    CommandQueue* GraphicsManager::GetComputeCommandQueue()
    {
        return g_GraphicsManager.m_ComputeCommandQueue.get();
    }

    CommandQueue* GraphicsManager::GetCopyCommandQueue()
    {
        return g_GraphicsManager.m_CopyCommandQueue.get();
    }

    SwapChain* GraphicsManager::GetSwapChain()
    {
        return g_GraphicsManager.m_SwapChain.get();
    }

    void GraphicsManager::Initialize()
    {
        g_GraphicsManager.Initialize();
    }

    void GraphicsManager::Destory()
    {
        g_GraphicsManager.Destory();
    }


}