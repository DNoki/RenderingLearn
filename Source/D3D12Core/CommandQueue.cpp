#include "pch.h"

#include "GraphicsCore.h"
#include "PipelineState.h"
#include "CommandList.h"

#include "CommandQueue.h"

// --------------------------------------------------------------------------
/*
    Direct3D 12 中的工作提交
    https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/command-queues-and-command-lists

    为了提高 Direct3D 应用的 CPU 效率，从版本 12 开始，Direct3D 不再支持与设备关联的即时上下文。 相反，应用程序会记录并提交“命令列表”，其中包含绘图和资源管理调用。 可以将这些命令列表从多个线程提交到一个或多个命令队列，命令队列用于管理命令的执行。 这种根本性的改变通过允许应用程序预先计算渲染工作以供以后重用，从而提高了单线程的效率，并且它通过将渲染工作分散到多个线程来利用多核系统。

    命令分配器（CommandAllocator）
        命令分配器可让应用管理分配给命令列表的内存，对应于存储 GPU 命令的底层分配。一个给定的分配器可同时与多个“当前正在记录”命令列表相关联，不过，可以使用一个命令分配器来创建任意数量的 GraphicsCommandList 对象。

    命令列表(CommandList)
        命令列表通常执行一次。 但是，如果应用程序在提交新执行之前确保先前的执行完成，则可以多次执行命令列表。
        任何线程都可以随时向任何命令队列提交命令列表，运行时将自动序列化命令队列中的命令列表提交，同时保留提交顺序。

    GPU 工作项的分组(Bundles)
        除了命令列表之外，Direct3D 12 通过添加第二级命令列表（称为bundles）来利用当今所有硬件中存在的功能。为了帮助区分这两种类型，可以将第一级命令列表称为直接命令列表。
        捆绑包的目的是允许应用程序将少量 API 命令组合在一起，以便以后从直接命令列表中重复执行。
        在创建 bundle 时，驱动程序将执行尽可能多的预处理，以提高后续执行的效率。然后可以从多个命令列表中执行捆绑包，并在同一命令列表中多次执行。

*/
// --------------------------------------------------------------------------

using namespace Graphics;

CommandQueue::CommandQueue() : m_Type(), m_CommandQueue(nullptr),
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


    // 创建同步对象 Fence， 用于等待渲染完成
    CHECK_HRESULT(g_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.put())));
    m_FenceValue = 1;

    // 创建用于帧同步的事件句柄，用于等待Fence事件通知
    m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_FenceEvent == nullptr)
        CHECK_HRESULT(HRESULT_FROM_WIN32(GetLastError()));
}

void CommandQueue::CloseQueue()
{
    if (m_IsClose) return;

    WaitForQueueCompleted();

    CloseHandle(m_FenceEvent);

    m_CommandQueue = nullptr;

    m_Fence = nullptr;

    m_IsClose = true;
}

void CommandQueue::ExecuteCommandLists(CommandList* commandLists, UINT numCommandLists)
{
    std::vector<ID3D12CommandList*> ppCommandLists(numCommandLists);

    for (UINT i = 0; i < numCommandLists; i++)
    {
        commandLists[0]->Close(); // 关闭列表以执行命令
        commandLists[0].SetLocked(true);
        ppCommandLists[i] = commandLists[0].GetD3D12CommandList();
    }
    m_CommandQueue->ExecuteCommandLists(numCommandLists, ppCommandLists.data());
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