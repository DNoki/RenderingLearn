#include "pch.h"
#include "​GraphicsCommand/CommandListPool.h"

using namespace D3D12Core;

class CommandListPoolImpl
{
public:
    CommandListPoolImpl() = default;

public:
    ICommandList* Request(D3D12_COMMAND_LIST_TYPE type)
    {
        Vector<UniquePtr<ICommandList>>* _list = nullptr;
        Queue<ICommandList*>* _queue = nullptr;
        ICommandList* result = nullptr;

        switch (type)
        {
        case D3D12_COMMAND_LIST_TYPE_DIRECT:
            _list = &m_GraphicsCommandLists;
            _queue = &m_GraphicsIdleQueue;
            break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE:
            _list = &m_ComputeCommandLists;
            _queue = &m_ComputeIdleQueue;
            break;
        case D3D12_COMMAND_LIST_TYPE_COPY:
            _list = &m_CopyCommandLists;
            _queue = &m_CopyIdleQueue;
            break;
        default:
            ASSERT(0, TEXT("ERROR::不支持的命令列表类型"));
            return result;
        }

        if (_queue->empty())
        {

            switch (type)
            {
            case D3D12_COMMAND_LIST_TYPE_DIRECT:
                _list->push_back(std::make_unique<GraphicsCommandList>());
                result = _list->back().get();
                static_cast<GraphicsCommandList*>(result)->Create();
                break;
            case D3D12_COMMAND_LIST_TYPE_COMPUTE:
                //_list->push_back(std::make_unique<ComputeCommandList>());
                //result = _list->back().get();
                //static_cast<ComputeCommandList*>(result)->Create();
                break;
            case D3D12_COMMAND_LIST_TYPE_COPY:
                //_list->push_back(std::make_unique<CopyCommandList>());
                //result = _list->back().get();
                //static_cast<CopyCommandList*>(result)->Create();
                break;
            default: return result;
            }
        }
        else
        {
            result = _queue->front();
            _queue->pop();
        }
        return result;
    }
    void Restore(ICommandList** commandList)
    {
        Queue<ICommandList*>* queqe = nullptr;

        switch ((*commandList)->GetType())
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
        default:
            ASSERT(0, L"ERROR::不支持的命令列表类型");
            break;
        }

        queqe->push(*commandList);
        *commandList = nullptr;
    }

private:
    Vector<UniquePtr<ICommandList>> m_GraphicsCommandLists{};
    Vector<UniquePtr<ICommandList>> m_ComputeCommandLists{};
    Vector<UniquePtr<ICommandList>> m_CopyCommandLists{};

    Queue<ICommandList*> m_GraphicsIdleQueue{};
    Queue<ICommandList*> m_ComputeIdleQueue{};
    Queue<ICommandList*> m_CopyIdleQueue{};

} g_CommandListPoolImpl;

void CommandListPool::Restore(ICommandList** commandList)
{
    g_CommandListPoolImpl.Restore(commandList);
}

ICommandList* CommandListPool::RequstImpl(D3D12_COMMAND_LIST_TYPE type)
{
    return g_CommandListPoolImpl.Request(type);
}
