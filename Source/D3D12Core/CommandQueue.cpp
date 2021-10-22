#include "pch.h"

#include "GraphicsCore.h"
#include "PipelineState.h"

#include "CommandQueue.h"

using namespace Graphics;

CommandQueue::CommandQueue() : m_Type(), m_CommandQueue(nullptr), m_CommandAllocator(nullptr), m_CommandList(nullptr),
m_Fence(nullptr), m_FenceValue(1), m_FenceEvent(nullptr),
m_IsClose(false)
{
}

void CommandQueue::Create(D3D12_COMMAND_LIST_TYPE type)
{
    m_IsClose = false;
    m_Type = type;

    // 创建D3D12命令队列接口
    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Type = m_Type; // 命令队列类型
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // 命令队列的优先级
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // 命令队列选项
    queueDesc.NodeMask = 1; // 节点标识
    CHECK_HRESULT(g_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CommandQueue.put())));


    // 创建命令列表分配器
    CHECK_HRESULT(g_Device->CreateCommandAllocator(m_Type, IID_PPV_ARGS(m_CommandAllocator.put())));


    // 创建同步对象 Fence， 用于等待渲染完成
    CHECK_HRESULT(g_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.put())));
    m_FenceValue = 1;

    // 创建用于帧同步的事件句柄，用于等待Fence事件通知
    m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_FenceEvent == nullptr)
        CHECK_HRESULT(HRESULT_FROM_WIN32(GetLastError()));
}

void CommandQueue::CreateCommandList(PipelineState* pso)
{
    // 创建图形命令列表
    CHECK_HRESULT(g_Device->CreateCommandList(0, m_Type, m_CommandAllocator.get(), pso->GetD3D12PSO(), IID_PPV_ARGS(m_CommandList.put())));
    // 命令列表是在录制状态下创建的，但还没有什么可录制的。 主循环希望它关闭，所以现在关闭它。
    m_CommandList->Close();
}

void CommandQueue::CloseQueue()
{
    if (m_IsClose) return;

    WaitForQueueCompleted();

    CloseHandle(m_FenceEvent);

    m_CommandQueue = nullptr;
    m_CommandAllocator = nullptr;
    m_CommandList = nullptr;
    m_Fence = nullptr;

    m_IsClose = true;
}

void CommandQueue::WaitForQueueCompleted()
{
    ASSERT(!m_IsClose);

    // 在继续之前等待框架完成不是最佳实践。 为简单起见，这是这样实现的代码。 D3D12HelloFrameBuffering 示例说明了如何使用围栏来有效利用资源并最大限度地提高 GPU 利用率。

    // 发出信号并增加围栏值。
    const UINT64 fence = m_FenceValue;
    CHECK_HRESULT(m_CommandQueue->Signal(m_Fence.get(), fence));
    m_FenceValue++;

    // 等到前一帧完成。
    auto completedValue = m_Fence->GetCompletedValue();
    if (completedValue < fence)
    {
        CHECK_HRESULT(m_Fence->SetEventOnCompletion(fence, m_FenceEvent));
        WaitForSingleObject(m_FenceEvent, INFINITE);
    }
}



//com_ptr<ID3D12CommandQueue> g_CommandQueue;
//winrt::com_ptr<ID3D12CommandAllocator> g_CommandAllocator;
//winrt::com_ptr<ID3D12GraphicsCommandList5> g_GraphicsCommandList;

//winrt::com_ptr<ID3D12Fence1> g_Fence;
//UINT64 FenceValue;
//HANDLE FenceEvent;


//// --------------------------------------------------------------------------
//// 创建D3D12命令队列接口
//D3D12_COMMAND_QUEUE_DESC queueDesc{};
//queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//CHECK_HRESULT(g_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_CommandQueue)));
//g_CommandQueue->SetName(L"Graphics Command Queue");

//// --------------------------------------------------------------------------
//// 创建命令列表分配器
//CHECK_HRESULT(g_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(g_CommandAllocator.put())));
//// 创建图形命令列表
//CHECK_HRESULT(g_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_CommandAllocator.get(), g_PipelineState->GetD3D12PSO(), IID_PPV_ARGS(g_GraphicsCommandList.put())));
//// 命令列表是在录制状态下创建的，但还没有什么可录制的。 主循环希望它关闭，所以现在关闭它。
//g_GraphicsCommandList->Close();


//// --------------------------------------------------------------------------
//// 创建同步对象 Fence， 用于等待渲染完成
//CHECK_HRESULT(g_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(g_Fence.put())));
//UINT64 FenceValue = 1;

//// 创建用于帧同步的事件句柄，用于等待Fence事件通知
//FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
//if (FenceEvent == nullptr)
//    CHECK_HRESULT(HRESULT_FROM_WIN32(GetLastError()));