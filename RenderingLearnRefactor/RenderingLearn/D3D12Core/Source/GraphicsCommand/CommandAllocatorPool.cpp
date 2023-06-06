#include "pch.h"
#include "​GraphicsCommand/CommandAllocatorPool.h"

#include "​GraphicsCommand/CommandAllocator.h"

using namespace D3D12Core;

class CommandAllocatorPoolImpl
{
public:
    CommandAllocatorPoolImpl() = default;

    /**
     * @brief 返回一个闲置分配器，若无闲置则创建它
     * @param type
     * @return
    */
    CommandAllocator* RequestAllocator(D3D12_COMMAND_LIST_TYPE type)
    {
        Vector<UniquePtr<CommandAllocator>>* _list = nullptr;
        Queue<CommandAllocator*>* _queue = nullptr;
        CommandAllocator* result = nullptr;

        switch (type)
        {
        case D3D12_COMMAND_LIST_TYPE_DIRECT:
            _list = &m_GraphicsCommandAllocators;
            _queue = &m_GraphicsIdleQueue;
            break;
        case D3D12_COMMAND_LIST_TYPE_BUNDLE:
            _list = &m_BundleCommandAllocators;
            _queue = &m_BundleIdleQueue;
            break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE:
            _list = &m_ComputeCommandAllocators;
            _queue = &m_ComputeIdleQueue;
            break;
        case D3D12_COMMAND_LIST_TYPE_COPY:
            _list = &m_CopyCommandAllocators;
            _queue = &m_CopyIdleQueue;
            break;
        default:
            ASSERT(0, L"ERROR::不支持的命令分配器类型");
            return result;
        }

        if (_queue->empty())
        {
            auto allocator = UniquePtr<CommandAllocator>(new CommandAllocator());
            allocator->Create(*GraphicsContext::GetCurrentInstance(), type);
            _list->push_back(std::move(allocator));
            result = _list->back().get();
            result->Reset();
        }
        else
        {
            result = _queue->front();
            _queue->pop();
            result->Reset();
        }
        return result;
    }

    /**
     * @brief 回收使用完毕的分配器
     * @param ca
    */
    void RestoreAllocator(CommandAllocator** ca)
    {
        Queue<CommandAllocator*>* _queue = nullptr;

        switch ((*ca)->GetType())
        {
        case D3D12_COMMAND_LIST_TYPE_DIRECT:
            _queue = &m_GraphicsIdleQueue;
            break;
        case D3D12_COMMAND_LIST_TYPE_BUNDLE:
            _queue = &m_BundleIdleQueue;
            break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE:
            _queue = &m_ComputeIdleQueue;
            break;
        case D3D12_COMMAND_LIST_TYPE_COPY:
            _queue = &m_CopyIdleQueue;
            break;
        default: ASSERT(0, L"ERROR::不支持的命令分配器类型"); break;
        }

        _queue->push(*ca);
        *ca = nullptr;
    }

private:
    Vector<UniquePtr<CommandAllocator>> m_GraphicsCommandAllocators{};   // 图形命令分配器
    Vector<UniquePtr<CommandAllocator>> m_BundleCommandAllocators{};     // 命令捆绑包分配器
    Vector<UniquePtr<CommandAllocator>> m_ComputeCommandAllocators{};    // 计算命令分配器
    Vector<UniquePtr<CommandAllocator>> m_CopyCommandAllocators{};       // 拷贝命令分配器

    Queue<CommandAllocator*> m_GraphicsIdleQueue{};   // 图形分配器等待队列
    Queue<CommandAllocator*> m_BundleIdleQueue{};     // 捆绑包分配器等待队列
    Queue<CommandAllocator*> m_ComputeIdleQueue{};    // 计算分配器等待队列
    Queue<CommandAllocator*> m_CopyIdleQueue{};       // 拷贝分配器等待队列


} g_CommandAllocatorPoolImpl;

CommandAllocator* CommandAllocatorPool::Request(D3D12_COMMAND_LIST_TYPE type)
{
    return nullptr;
}
void CommandAllocatorPool::Restore(CommandAllocator** allocator)
{

}
