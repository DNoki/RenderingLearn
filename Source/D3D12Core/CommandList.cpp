#include "pch.h"

#include "GraphicsCore.h"
#include "PipelineState.h"
#include "CommandQueue.h"

#include "CommandList.h"

using namespace Graphics;

CommandList::CommandList() : m_Type(), m_CommandAllocator(), m_CommandList(), m_IsLocked()
{
}

void CommandList::Create(D3D12_COMMAND_LIST_TYPE type)
{
    ASSERT(type != D3D12_COMMAND_LIST_TYPE_BUNDLE, L"命令列表暂不支持指定类型。");
    m_Type = type;

    // 创建命令列表分配器
    CHECK_HRESULT(g_Device->CreateCommandAllocator(m_Type, IID_PPV_ARGS(m_CommandAllocator.put())));


    // 创建命令列表
    // 使用 CreateCommandList1 可以直接创建关闭的命令列表，而无需传入管线状态对象
    CHECK_HRESULT(g_Device->CreateCommandList1(1,
        m_Type,
        D3D12_COMMAND_LIST_FLAG_NONE,
        IID_PPV_ARGS(m_CommandList.put())));
    //CHECK_HRESULT(g_Device->CreateCommandList(1,
    //    m_Type,                     // 命令列表类型
    //    m_CommandAllocator.get(),   // 命令列表分配器
    //    nullptr,                    // 初始管线状态为NULL
    //    IID_PPV_ARGS(m_CommandList.put())));


    // 指示列表可以写入命令
    m_IsLocked = true;
}

void CommandList::Reset(const PipelineState* pso)
{
    // 在应用程序调用Reset之前，命令列表必须处于“关闭”状态。
    ASSERT(m_IsLocked);

    // 命令列表分配器只能在相关命令列表在 GPU 上完成执行时重置, 应用程序应使用围栏来确定 GPU 执行进度。
    CHECK_HRESULT(m_CommandAllocator->Reset());

    // 将命令列表重置回其初始状态
    // TODO 通过使用Reset，您可以重用命令列表跟踪结构而无需任何分配。与ID3D12CommandAllocator::Reset不同，您可以在命令列表仍在执行时调用Reset。一个典型的模式是提交一个命令列表，然后立即重置它以将分配的内存重新用于另一个命令列表。
    CHECK_HRESULT(m_CommandList->Reset(m_CommandAllocator.get(), pso ? pso->GetD3D12PSO() : nullptr));

    // 指示列表可以写入命令
    m_IsLocked = false;
}
