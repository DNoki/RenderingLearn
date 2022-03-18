#include "pch.h"

#include "GraphicsManager.h"


#include "CommandAllocatorPool.h"


// --------------------------------------------------------------------------
/*
    命令分配器（CommandAllocator）
        命令分配器可让应用管理分配给命令列表的内存，对应于存储 GPU 命令的底层分配。一个给定的分配器可同时与多个“当前正在记录”命令列表相关联，不过，可以使用一个命令分配器来创建任意数量的 GraphicsCommandList 对象。


    设计思路：
        使用对象池来管理命令分配器，
*/
// --------------------------------------------------------------------------

using namespace std;
using namespace winrt;

namespace Graphics
{
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
            vector<unique_ptr<CommandAllocator>>* _list = nullptr;
            queue<CommandAllocator*>* _queue = nullptr;
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
                _list->push_back(unique_ptr<CommandAllocator>(new CommandAllocator(type)));
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
            queue<CommandAllocator*>* _queue = nullptr;

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
        vector<unique_ptr<CommandAllocator>> m_GraphicsCommandAllocators{};   // 图形命令分配器
        vector<unique_ptr<CommandAllocator>> m_BundleCommandAllocators{};     // 命令捆绑包分配器
        vector<unique_ptr<CommandAllocator>> m_ComputeCommandAllocators{};    // 计算命令分配器
        vector<unique_ptr<CommandAllocator>> m_CopyCommandAllocators{};       // 拷贝命令分配器

        queue<CommandAllocator*> m_GraphicsIdleQueue{};   // 图形分配器等待队列
        queue<CommandAllocator*> m_BundleIdleQueue{};     // 捆绑包分配器等待队列
        queue<CommandAllocator*> m_ComputeIdleQueue{};    // 计算分配器等待队列
        queue<CommandAllocator*> m_CopyIdleQueue{};       // 拷贝分配器等待队列


    } g_CommandAllocatorPoolImpl;


    CommandAllocator::CommandAllocator(D3D12_COMMAND_LIST_TYPE type) : m_Type(type)
    {
        // 创建命令列表分配器
        CHECK_HRESULT(GraphicsManager::GetDevice()->CreateCommandAllocator(type, IID_PPV_ARGS(m_CommandAllocator.put())));

        if (D3D12_COMMAND_LIST_TYPE_DIRECT <= type && type < D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE)
        {
            static const wstring names[] = { L"Graphics", L"Bundle", L"Compute", L"Copy", };
            SET_DEBUGNAME(m_CommandAllocator.get(), Application::Format(L"%s (CommandAllocator)", names[type].c_str()));
        }
    }


    CommandAllocator* CommandAllocatorPool::Request(D3D12_COMMAND_LIST_TYPE type)
    {
        return g_CommandAllocatorPoolImpl.RequestAllocator(type);
    }
    void CommandAllocatorPool::Restore(CommandAllocator** allocator)
    {
        // 回收此命令分配器，必须保证该分配器已执行完毕
        // 命令列表分配器只能在相关命令列表在 GPU 上完成执行时重置, 应用程序应使用围栏来确定 GPU 执行进度。
        g_CommandAllocatorPoolImpl.RestoreAllocator(allocator);
    }
}