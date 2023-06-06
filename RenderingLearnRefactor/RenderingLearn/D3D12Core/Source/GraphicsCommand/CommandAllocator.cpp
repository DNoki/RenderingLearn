#include "pch.h"
#include "​GraphicsCommand/CommandAllocator.h"


// --------------------------------------------------------------------------
/*
    命令分配器（CommandAllocator）
        命令分配器可让应用管理分配给命令列表的内存，对应于存储 GPU 命令的底层分配。一个给定的分配器可同时与多个“当前正在记录”命令列表相关联，不过，可以使用一个命令分配器来创建任意数量的 GraphicsCommandList 对象。


    设计思路：
        使用对象池来管理命令分配器，
*/
// --------------------------------------------------------------------------

using namespace D3D12Core;

void CommandAllocator::Create(const GraphicsContext& context, D3D12_COMMAND_LIST_TYPE type)
{
    // 创建命令列表分配器
    CHECK_HRESULT(context.GetDevice()->CreateCommandAllocator(type, IID_PPV_ARGS(m_CommandAllocator.put())));

    if (D3D12_COMMAND_LIST_TYPE_DIRECT <= type && type < D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE)
    {
        static const String names[] =
        {
            TEXT("Graphics"),
            TEXT("Bundle"),
            TEXT("Compute"),
            TEXT("Copy"),
        };
        GraphicsContext::SetDebugName(m_CommandAllocator.get(), FORMAT(TEXT("%s (CommandAllocator)"), names[type].c_str()));
    }
}
