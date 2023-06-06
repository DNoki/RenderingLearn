#include "pch.h"
#include "System/GraphicsManager.h"

using namespace D3D12Core;
using namespace D3D12Viewer;

GraphicsManager GraphicsManager::s_Instance = GraphicsManager();

void GraphicsManager::Initialize(HWND windowHandle)
{
    m_GraphicsContext = std::make_unique<GraphicsContext>();
    m_GraphicsContext->Initialize();
    m_GraphicsContext->SetAltEnterEnable(windowHandle, false);

    // 创建图形命令队列
    {
        m_GraphicsCommandQueue.reset(new CommandQueue());
        m_ComputeCommandQueue.reset(new CommandQueue());
        m_CopyCommandQueue.reset(new CommandQueue());
        m_GraphicsCommandQueue->Create(*m_GraphicsContext, D3D12_COMMAND_LIST_TYPE_DIRECT);
        m_ComputeCommandQueue->Create(*m_GraphicsContext, D3D12_COMMAND_LIST_TYPE_COMPUTE);
        m_CopyCommandQueue->Create(*m_GraphicsContext, D3D12_COMMAND_LIST_TYPE_COPY);
    }

    // 初始化交换链
    m_SwapChain.reset(new SwapChain());
    m_SwapChain->CreateForHwnd(*m_GraphicsContext, m_GraphicsCommandQueue.get(), windowHandle, SwapFrameBackBufferCount, SwapChainRenderTargetFormat);
}

void GraphicsManager::Destory()
{
    m_SwapChain = nullptr;

    // 等待队列指令完成后关闭
    m_GraphicsCommandQueue->CloseQueue();
    m_ComputeCommandQueue->CloseQueue();
    m_CopyCommandQueue->CloseQueue();

    m_GraphicsContext->Destroy();
}
