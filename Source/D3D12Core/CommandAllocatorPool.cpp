#include "pch.h"

#include "GraphicsCore.h"


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
        CommandAllocatorPoolImpl() :
            m_GraphicsCommandAllocators(), m_ComputeCommandAllocators(), m_CopyCommandAllocators(),
            m_GraphicsIdleQueue(), m_ComputeIdleQueue(), m_CopyIdleQueue()
        {

        }

        /**
         * @brief 返回一个闲置分配器，若无闲置则创建它
         * @param type
         * @return
        */
        CommandAllocator* RequestAllocator(D3D12_COMMAND_LIST_TYPE type)
        {
            vector<CommandAllocator>* list = nullptr;
            queue<CommandAllocator*>* queqe = nullptr;
            CommandAllocator* result = nullptr;

            switch (type)
            {
            case D3D12_COMMAND_LIST_TYPE_DIRECT:
                list = &m_GraphicsCommandAllocators;
                queqe = &m_GraphicsIdleQueue;
                break;
            case D3D12_COMMAND_LIST_TYPE_COMPUTE:
                list = &m_ComputeCommandAllocators;
                queqe = &m_ComputeIdleQueue;
                break;
            case D3D12_COMMAND_LIST_TYPE_COPY:
                list = &m_CopyCommandAllocators;
                queqe = &m_CopyIdleQueue;
                break;
            default:
                ASSERT(0, L"ERROR::不支持的命令分配器类型");
                return result;
            }

            if (queqe->empty())
            {
                list->push_back(CommandAllocator(type));
                result = &list->back();
                result->GetD3D12Allocator()->Reset();
            }
            else
            {
                result = queqe->front();
                queqe->pop();
                result->GetD3D12Allocator()->Reset();
            }
            return result;
        }

        /**
         * @brief 回收使用完毕的分配器
         * @param ca
        */
        void RestoreAllocator(CommandAllocator* ca)
        {
            queue<CommandAllocator*>* queqe = nullptr;

            switch (ca->GetType())
            {
            case D3D12_COMMAND_LIST_TYPE_DIRECT:
                queqe = &m_GraphicsIdleQueue;
                break;
            case D3D12_COMMAND_LIST_TYPE_COMPUTE:
                queqe = &m_ComputeIdleQueue;
                break;
            case D3D12_COMMAND_LIST_TYPE_COPY:
                queqe = &m_CopyIdleQueue;
                break;
            default: ASSERT(0, L"ERROR::不支持的命令分配器类型"); break;
            }

            queqe->push(ca);
        }

    private:
        vector<CommandAllocator> m_GraphicsCommandAllocators;
        vector<CommandAllocator> m_ComputeCommandAllocators;
        vector<CommandAllocator> m_CopyCommandAllocators;

        queue<CommandAllocator*> m_GraphicsIdleQueue;
        queue<CommandAllocator*> m_ComputeIdleQueue;
        queue<CommandAllocator*> m_CopyIdleQueue;


    } g_CommandAllocatorPoolImpl;


    CommandAllocator::CommandAllocator(D3D12_COMMAND_LIST_TYPE type) : m_Type(type), m_CommandAllocator()
    {
        // 创建命令列表分配器
        CHECK_HRESULT(g_Device->CreateCommandAllocator(type, IID_PPV_ARGS(m_CommandAllocator.put())));
    }

    void CommandAllocator::Restore()
    {
        // 回收此命令分配器，必须保证该分配器已执行完毕
        // 命令列表分配器只能在相关命令列表在 GPU 上完成执行时重置, 应用程序应使用围栏来确定 GPU 执行进度。
        g_CommandAllocatorPoolImpl.RestoreAllocator(this);
    }


    CommandAllocator* CommandAllocatorPool::Request(D3D12_COMMAND_LIST_TYPE type)
    {
        return g_CommandAllocatorPoolImpl.RequestAllocator(type);
    }
}