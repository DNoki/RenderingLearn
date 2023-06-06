#include "pch.h"

#include "​GraphicsCommand/CommandAllocatorPool.h"

using namespace std;
using namespace winrt;

namespace D3D12Core
{
    CommandAllocator* CommandAllocatorPool::Request(D3D12_COMMAND_LIST_TYPE type)
    {
        return nullptr;
    }
    void CommandAllocatorPool::Restore(CommandAllocator** allocator)
    {

    }
}