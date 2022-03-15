#include "pch.h"

#include "GraphicsManager.h"
#include "SwapChain.h"
#include "RenderTexture.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "DescriptorHeap.h"
#include "GraphicsResource.h"
#include "CommandQueue.h"
#include "CommandAllocatorPool.h"

#include "CommandList.h"


// --------------------------------------------------------------------------
/*
    创建和记录命令列表和捆绑包 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/recording-command-lists-and-bundles

    命令列表(CommandList)
        命令列表通常执行一次。 但是，如果应用程序在提交新执行之前确保先前的执行完成，则可以多次执行命令列表。
        任何线程都可以随时向任何命令队列提交命令列表，运行时将自动序列化命令队列中的命令列表提交，同时保留提交顺序。

    GPU 工作项的分组(Bundles)
        除了命令列表之外，Direct3D 12 通过添加第二级命令列表（称为bundles）来利用当今所有硬件中存在的功能。为了帮助区分这两种类型，可以将第一级命令列表称为直接命令列表。
        捆绑包的目的是允许应用程序将少量 API 命令组合在一起，以便以后从直接命令列表中重复执行。
        在创建 bundle 时，驱动程序将执行尽可能多的预处理，以提高后续执行的效率。然后可以从多个命令列表中执行捆绑包，并在同一命令列表中多次执行。

    图形管线
        DX11 https://docs.microsoft.com/zh-cn/windows/win32/direct3d11/overviews-direct3d-11-graphics-pipeline
        DX12 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/pipelines-and-shaders-with-directx-12
        IA：Input-Assembler
        SO：Stream Output
        RS：Rasterizer Stage
        OM：Output Merger

*/
// --------------------------------------------------------------------------

using namespace std;

namespace Graphics
{
    CommandList::~CommandList()
    {
        if (m_Type == D3D12_COMMAND_LIST_TYPE_BUNDLE)
        {
            // 回收捆绑包使用的分配器
            ASSERT(m_CommandAllocator);
            CommandAllocatorPool::Restore(m_CommandAllocator);
        }
    }

    void CommandList::Create(D3D12_COMMAND_LIST_TYPE type, bool isAllocate, const PipelineState* pso)
    {
        m_Type = type;

        if (isAllocate)
        {
            // 返回一个命令分配器
            m_CommandAllocator = CommandAllocatorPool::Request(m_Type);

            CHECK_HRESULT(GraphicsManager::GetDevice()->CreateCommandList(
                NODEMASK,
                m_Type,                     // 命令列表类型
                m_CommandAllocator->GetD3D12Allocator(),    // 命令列表分配器
                pso ? pso->GetD3D12PSO() : nullptr,         // 初始管线状态为NULL
                IID_PPV_ARGS(m_CommandList.put())));

            // 指示列表可以写入命令
            m_IsLocked = false;
        }
        else
        {
            // 创建命令列表
            // 使用 CreateCommandList1 可以直接创建关闭的命令列表，而无需传入管线状态对象
            CHECK_HRESULT(GraphicsManager::GetDevice()->CreateCommandList1(
                NODEMASK,
                m_Type,
                D3D12_COMMAND_LIST_FLAG_NONE,
                IID_PPV_ARGS(m_CommandList.put())));

            // 指示列表处于关闭状态
            m_IsLocked = true;
            m_CommandAllocator = nullptr;
        }
        SET_DEBUGNAME(m_CommandList.get(), _T("CommandList"));
    }

    void CommandList::Close()
    {
        ASSERT(!m_IsLocked);
        CHECK_HRESULT(m_CommandList->Close());
        m_IsLocked = true;

        // 命令写入分配器之后，解除之前绑定的分配器
        // 对于捆绑包来说，一般不会对其重置，所以使用保持对分配器的引用
        if (m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE)
            m_CommandAllocator = nullptr;
    }

    void CommandList::Reset(const PipelineState* pso)
    {
        // 在应用程序调用Reset之前，命令列表必须处于“关闭”状态。
        ASSERT(m_IsLocked);

        // 返回一个命令分配器
        if (!m_CommandAllocator)
            m_CommandAllocator = CommandAllocatorPool::Request(m_Type);
        else if (m_Type == D3D12_COMMAND_LIST_TYPE_BUNDLE)
        {
            CommandAllocatorPool::Restore(m_CommandAllocator);
            m_CommandAllocator = CommandAllocatorPool::Request(m_Type);
        }

        // 将命令列表重置回其初始状态
        // TODO 通过使用Reset，您可以重用命令列表跟踪结构而无需任何分配。与ID3D12CommandAllocator::Reset不同，您可以在命令列表仍在执行时调用Reset。一个典型的模式是提交一个命令列表，然后立即重置它以将分配的内存重新用于另一个命令列表。
        CHECK_HRESULT(m_CommandList->Reset(m_CommandAllocator->GetD3D12Allocator(), pso ? pso->GetD3D12PSO() : nullptr));

        // 指示列表可以写入命令
        m_IsLocked = false;
    }

    void CommandList::AddOnCompletedEvent(std::function<void()> onCompleted) const
    {
        ASSERT(m_CommandAllocator);
        if (m_CommandAllocator)
            m_CommandAllocator->m_OnCompletedEvents.push_back(onCompleted);
    }

    void CommandList::ResourceTransitionBarrier(const GraphicsResource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) const
    {
        /*
            资源屏障同步资源状态 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/using-resource-barriers-to-synchronize-resource-states-in-direct3d-12
        */
        ASSERT(m_Type != D3D12_COMMAND_LIST_TYPE_BUNDLE);
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource->GetD3D12Resource(), before, after);
        m_CommandList->ResourceBarrier(1, &barrier);
    }

    void CommandList::SetGraphicsRootSignature(const RootSignature* pRootSignature) const
    {
        m_CommandList->SetGraphicsRootSignature(pRootSignature->GetD3D12RootSignature());
    }
    void CommandList::SetPipelineState(const PipelineState* pPipelineState) const
    {
        m_CommandList->SetPipelineState(pPipelineState->GetD3D12PSO());
    }
    void CommandList::SetDescriptorHeaps(const DescriptorHeap* pResourceDescHeap, const DescriptorHeap* pSamplerDescHeap) const
    {
        /*
            命令列表只能绑定 CBV_SRV_UAV 或 SAMPLER 类型描述符堆
            每种类型的描述符堆一次只能设置一个，即一次最多可以设置2个堆
        */
        std::vector<ID3D12DescriptorHeap*> descHeaps{};
        if (pResourceDescHeap)
            descHeaps.push_back(pResourceDescHeap->GetD3D12DescriptorHeap());
        if (pSamplerDescHeap)
            descHeaps.push_back(pSamplerDescHeap->GetD3D12DescriptorHeap());

        ASSERT(descHeaps.size() > 0);
        m_CommandList->SetDescriptorHeaps(static_cast<UINT>(descHeaps.size()), descHeaps.data());
    }

    void CommandList::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, const DescriptorHeap* descriptorHeap) const
    {
        m_CommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, descriptorHeap->GetDescriptorHandle(0));
    }
    void CommandList::SetGraphicsRootConstantBufferView(UINT rootParameterIndex, const IBufferResource* buffer) const
    {
        m_CommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, buffer->GetGpuVirtualAddress());
    }

    class CommandListPoolImpl
    {
    public:
        CommandListPoolImpl() = default;

        inline CommandList* Request(D3D12_COMMAND_LIST_TYPE type)
        {
            vector<unique_ptr<CommandList>>* list = nullptr;
            queue<CommandList*>* queqe = nullptr;
            CommandList* result = nullptr;

            switch (type)
            {
            case D3D12_COMMAND_LIST_TYPE_DIRECT:
                list = &m_GraphicsCommandLists;
                queqe = &m_GraphicsIdleQueue;
                break;
            case D3D12_COMMAND_LIST_TYPE_COMPUTE:
                list = &m_ComputeCommandLists;
                queqe = &m_ComputeIdleQueue;
                break;
            case D3D12_COMMAND_LIST_TYPE_COPY:
                list = &m_CopyCommandLists;
                queqe = &m_CopyIdleQueue;
                break;
            default:
                ASSERT(0, L"ERROR::不支持的命令列表类型");
                return result;
            }

            if (queqe->empty())
            {
                list->push_back(unique_ptr<CommandList>(new CommandList()));
                result = list->back().get();
                result->Create(type);
            }
            else
            {
                result = queqe->front();
                queqe->pop();
            }
            return result;
        }
        inline void Restore(CommandList* commandList)
        {
            queue<CommandList*>* queqe = nullptr;

            switch (commandList->GetType())
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

            queqe->push(commandList);
        }

    private:
        vector<unique_ptr<CommandList>> m_GraphicsCommandLists{};
        vector<unique_ptr<CommandList>> m_ComputeCommandLists{};
        vector<unique_ptr<CommandList>> m_CopyCommandLists{};

        queue<CommandList*> m_GraphicsIdleQueue{};
        queue<CommandList*> m_ComputeIdleQueue{};
        queue<CommandList*> m_CopyIdleQueue{};

    } g_CommandListPoolImpl;

    CommandList* CommandListPool::Request(D3D12_COMMAND_LIST_TYPE type)
    {
        return g_CommandListPoolImpl.Request(type);
    }
    void CommandListPool::Restore(CommandList* commandList)
    {
        g_CommandListPoolImpl.Restore(commandList);
    }
}