#include "pch.h"

#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "GraphicsCommon.h"

#include "PipelineState.h"

#include "Display.h"
#include "CommandQueue.h"

#include "SampleResource.h"
#include "AppMain.h"

#include "GraphicsCore.h"


using namespace winrt;
using namespace std;
using namespace Display;


namespace Graphics
{
    com_ptr<IDXGIFactory7> g_Factory;
    com_ptr<IDXGIAdapter4> g_Adapter;
    com_ptr<ID3D12Device6> g_Device;
    com_ptr<IDXGISwapChain4> g_SwapChain;

    unique_ptr<DescriptorHeap> g_RTVDescriptorHeap;
    vector<unique_ptr<RenderTexture>> g_RenderTargets;

    unique_ptr<CommandQueue> g_GraphicsCommandQueue;


    UINT64 g_FrameCount; // 已渲染帧数量


    void InitializeCommonSampler();

    void Initialize()
    {
        ASSERT(g_Factory == nullptr);
        ASSERT(g_Device == nullptr);
        UINT nDXGIFactoryFlags = 0U;
        g_FrameCount = 0u;

        // --------------------------------------------------------------------------
        // 启用调试层
#ifdef _DEBUG
        // 启用调试层（需要图形工具“可选功能”）。
        // 注意：在设备创建后启用调试层将使活动设备无效。
        com_ptr<ID3D12Debug3> pID3D12Debug3;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(pID3D12Debug3.put()))))
        {
            pID3D12Debug3->EnableDebugLayer();
            nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
#endif // _DEBUG


        // --------------------------------------------------------------------------
        // 创建 DirectX 图形基础设施 (DXGI) 
        CHECK_HRESULT(CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(g_Factory.put())));
        //CHECK_HRESULT(pFactory->MakeWindowAssociation(g_hwnd, DXGI_MWA_NO_ALT_ENTER)); // 防止 DXGI 响应 Alt+Enter 


        // --------------------------------------------------------------------------
        // 创建D3D12设备对象接口
        com_ptr<IDXGIAdapter4> pAdapter;
        com_ptr<ID3D12Device6> pDevice;
        SIZE_T MaxSize = 0;
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != g_Factory->EnumAdapters1(adapterIndex, (IDXGIAdapter1**)(pAdapter.put())); adapterIndex++)
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
                    g_Adapter = move(pAdapter);
                }
            }
            // 重新设定智能指针
            pAdapter = nullptr;
        }
        if (MaxSize > 0)
            g_Device = move(pDevice);
        ASSERT(g_Device != nullptr, L"D3D12设备对象创建失败");

        DXGI_QUERY_VIDEO_MEMORY_INFO GpuMemoryInfo{}; // TODO 描述当前的GPU内存预算参数
        CHECK_HRESULT(g_Adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &GpuMemoryInfo));
        {
            auto title = Application::GetWindowTitle();
            DXGI_ADAPTER_DESC3 desc;
            g_Adapter->GetDesc3(&desc);
            Application::SetWindowTitle(Utility::Format(L"%s  GPU(%s)", title.c_str(), desc.Description).c_str());
        }

        // --------------------------------------------------------------------------
        // 创建图形命令队列
        g_GraphicsCommandQueue = unique_ptr<CommandQueue>(new CommandQueue());
        g_GraphicsCommandQueue->Create(D3D12_COMMAND_LIST_TYPE_DIRECT);


        // --------------------------------------------------------------------------
        // 初始化交换链
        Display::Initialize();
        // --------------------------------------------------------------------------
        // 创建描述符堆
        g_RTVDescriptorHeap = unique_ptr<DescriptorHeap>(new DescriptorHeap());
        g_RTVDescriptorHeap->Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, Display::SWAP_FRAME_BACK_BUFFER_COUNT);
        // --------------------------------------------------------------------------
        // 创建RTV描述符  RTV（渲染目标视图）
        g_RenderTargets.clear();
        for (UINT i = 0; i < Display::SWAP_FRAME_BACK_BUFFER_COUNT; i++)
        {
            g_RenderTargets.push_back(unique_ptr<RenderTexture>(new RenderTexture()));
            auto& rt = g_RenderTargets[i];
            rt->CreateFromSwapChain(i, nullptr, g_RTVDescriptorHeap->GetDescriptorHandle(i));
        }

        // --------------------------------------------------------------------------
        // 初始化共通动态采样器
        InitializeCommonSampler();


        // --------------------------------------------------------------------------
        // 创建示例PSO
        SampleResource::InitRootSignature();
        SampleResource::InitPipelineState();


        // --------------------------------------------------------------------------
        // 创建指令列表
        g_GraphicsCommandQueue->CreateCommandList(&SampleResource::g_PipelineState);


        // --------------------------------------------------------------------------
        // 创建示例资源
        SampleResource::InitPlacedHeap();
        SampleResource::InitMesh();
        SampleResource::InitTexture2D();

        // --------------------------------------------------------------------------
        // 由于初始化贴图时需要执行复制命令
        // 关闭命令列表并执行它以开始初始 GPU 设置。
        g_GraphicsCommandQueue->GetD3D12CommandList()->Close();
        // 执行命令列表
        ID3D12CommandList* ppCommandLists[] = { g_GraphicsCommandQueue->GetD3D12CommandList() };
        g_GraphicsCommandQueue->GetD3D12CommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // 等待命令列表执行。 我们在主循环中重用相同的命令列表，但现在，我们只想等待设置完成后再继续。
        g_GraphicsCommandQueue->WaitForQueueCompleted();
    }


    void OnRender()
    {
        // --------------------------------------------------------------------------
            // 渲染
            // 填充命令列表
        {
            // 命令列表分配器只能在相关命令列表在 GPU 上完成执行时重置, 应用程序应使用围栏来确定 GPU 执行进度。
            CHECK_HRESULT(g_GraphicsCommandQueue->GetD3D12CommandAllocator()->Reset());

            // 获取当前后台缓冲索引
            g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

            // 当对特定命令列表调用 ExecuteCommandList() 时，该命令列表可以随时重置，并且必须在重新录制之前重置。
            auto currentCommandList = g_GraphicsCommandQueue->GetD3D12CommandList();
            CHECK_HRESULT(currentCommandList->Reset(g_GraphicsCommandQueue->GetD3D12CommandAllocator(), SampleResource::g_PipelineState.GetD3D12PSO()));

            // 设置必要的状态。
            currentCommandList->SetGraphicsRootSignature(SampleResource::g_PipelineState.GetD3D12RootSignature());
            auto viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(Display::GetScreenWidth()), static_cast<float>(Display::GetScreenHeight()));
            currentCommandList->RSSetViewports(1, &viewport);
            auto scissorRect = CD3DX12_RECT(0, 0, Display::GetScreenWidth(), Display::GetScreenHeight());
            currentCommandList->RSSetScissorRects(1, &scissorRect);

            // 指示后台缓冲区将用作渲染目标。
            auto& currentRenderTarget = *(g_RenderTargets[g_CurrentBackBufferIndex].get());
            auto barriers1 = CD3DX12_RESOURCE_BARRIER::Transition(currentRenderTarget.GetD3D12Resource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            currentCommandList->ResourceBarrier(1, &barriers1);

            // 设置渲染目标
            currentCommandList->OMSetRenderTargets(1, currentRenderTarget.GetDescriptorHandle()->GetCpuHandle(), FALSE, nullptr);


            // 记录命令
            const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
            currentCommandList->ClearRenderTargetView(*(currentRenderTarget.GetDescriptorHandle()), clearColor, 0, nullptr);

            SampleResource::SampleDraw(currentCommandList);


            // 指示现在将使用后台缓冲区来呈现。
            auto barriers2 = CD3DX12_RESOURCE_BARRIER::Transition(currentRenderTarget.GetD3D12Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            currentCommandList->ResourceBarrier(1, &barriers2);

            CHECK_HRESULT(currentCommandList->Close());
        }

        // 执行命令列表
        ID3D12CommandList* ppCommandLists[] = { g_GraphicsCommandQueue->GetD3D12CommandList() };
        g_GraphicsCommandQueue->GetD3D12CommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // 呈现帧。
        CHECK_HRESULT(g_SwapChain->Present(1, 0));

        g_GraphicsCommandQueue->WaitForQueueCompleted();

        g_FrameCount++;
    }

    void OnDestroy()
    {
        // 等待队列指令完成后关闭
        g_GraphicsCommandQueue->CloseQueue();
    }

}