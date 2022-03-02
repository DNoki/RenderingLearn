#include "pch.h"

#include "GraphicsCore.h"
#include "PipelineState.h"
#include "CommandAllocatorPool.h"
#include "CommandList.h"

#include "CommandQueue.h"

// --------------------------------------------------------------------------
/*
    Direct3D 12 中的工作提交
    https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/command-queues-and-command-lists

    命令队列
        为了提高 Direct3D 应用的 CPU 效率，从版本 12 开始，Direct3D 不再支持与设备关联的即时上下文。 相反，应用程序会记录并提交“命令列表”，其中包含绘图和资源管理调用。 可以将这些命令列表从多个线程提交到一个或多个命令队列，命令队列用于管理命令的执行。 这种根本性的改变通过允许应用程序预先计算渲染工作以供以后重用，从而提高了单线程的效率，并且它通过将渲染工作分散到多个线程来利用多核系统。

        Direct3D 12 命令队列将以前不会向开发人员公开的即时模式工作提交内容的运行时和驱动程序同步，替换为可显式管理并发性、并行度和同步的 API。

        命令队列为开发人员提供以下方面的改进：
            ·可让开发人员避免意外的同步导致效率意外下降的问题。
            ·可让开发人员引入更高级别的同步，以便更有效、更准确地确定所需的同步。 这意味着，运行时和图形驱动程序可以减少被动式工程并行度的时间。
            ·使高开销的操作更为明确。
        这些改进实现或增强了以下方案：
            ·提高并行度 - 如果应用程序为前台工作提供独立的队列，则可以使用更深层的队列来完成后台工作负荷（例如视频解码）。
            ·异步和低优先级 GPU 工作 - 命令队列模型可以实现低优先级 GPU 工作和原子操作的并发执行，这些操作支持在不阻塞的情况下，通过一个 GPU 线程来使用另一个未同步线程的结果。
            ·高优先级计算工作 - 使用此设计可实现以下方案：中断 3D 渲染来执行少量的高优先级计算工作，可以提前获取结果，以便在 CPU 上进行其他处理。


*/
// --------------------------------------------------------------------------

namespace Graphics
{
    CommandQueue::CommandQueue() : m_Type(), m_CommandQueue(nullptr),
        m_Fence(nullptr), m_FenceValue(1), m_FenceEvent(nullptr)
    {
    }

    void CommandQueue::Create(D3D12_COMMAND_LIST_TYPE type)
    {
        m_Type = type;

        // 创建D3D12命令队列接口
        D3D12_COMMAND_QUEUE_DESC queueDesc{};
        queueDesc.Type = m_Type; // 命令队列类型
        queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // 命令队列的优先级
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // 命令队列选项
        queueDesc.NodeMask = NODEMASK; // 节点标识
        CHECK_HRESULT(GraphicsManager::GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CommandQueue.put())));
        SET_DEBUGNAME(m_CommandQueue.get(), _T("CommandQueue"));


        // 创建同步对象 Fence， 用于等待渲染完成
        CHECK_HRESULT(GraphicsManager::GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.put())));
        m_FenceValue = 1;

        // 创建用于帧同步的事件句柄，用于等待Fence事件通知
        m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_FenceEvent == nullptr)
            CHECK_HRESULT(HRESULT_FROM_WIN32(GetLastError()));
    }

    void CommandQueue::CloseQueue()
    {
        WaitForQueueCompleted();

        CloseHandle(m_FenceEvent);

        m_CommandQueue = nullptr;
        m_Fence = nullptr;
    }

    void CommandQueue::ExecuteCommandLists(CommandList* commandLists, UINT numCommandLists)
    {
        std::vector<ID3D12CommandList*> ppCommandLists(numCommandLists);

        for (UINT i = 0; i < numCommandLists; i++)
        {
            ASSERT(m_Type == commandLists[i].GetType(), L"ERROR::命令列表类型与队列类型不一致。");
            ASSERT(!commandLists[i].IsLocked()); // 设只允许队列来关闭命令列表

            m_Allocators.push_back(commandLists[i].GetCommandAllocator()); // 添加分配器到使用中
            commandLists[i].Close(); // 关闭列表以执行命令
            ppCommandLists[i] = commandLists[i].GetD3D12CommandList();
        }
        m_CommandQueue->ExecuteCommandLists(numCommandLists, ppCommandLists.data());
        CommandListPool::Restore(commandLists); // 将使用完毕的列表放回池
    }

    void CommandQueue::WaitForQueueCompleted()
    {
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

        // 将所有使用的分配器释放
        for (auto allocator : m_Allocators)
        {
            CommandAllocatorPool::Restore(allocator);
        }
        m_Allocators.clear();
    }
}
