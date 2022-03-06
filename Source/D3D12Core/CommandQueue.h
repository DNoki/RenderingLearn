#pragma once

namespace Graphics
{
    class PipelineState;
    class CommandAllocator;
    class CommandList;

    /**
     * @brief 指令队列
    */
    class CommandQueue
    {
    public:
        CommandQueue() = default;

        /**
         * @brief 创建指令队列
         * @param type 指令列表类型
        */
        void Create(D3D12_COMMAND_LIST_TYPE type);

        void ExecuteCommandLists(CommandList* commandLists, UINT numCommandLists = 1);

        /**
         * @brief 等待队列完成
        */
        void WaitForQueueCompleted();

        /**
         * @brief 关闭指令队列并释放资源
        */
        void CloseQueue();

        inline ID3D12CommandQueue* GetD3D12CommandQueue() const { return m_CommandQueue.get(); }

    private:
        winrt::com_ptr<ID3D12CommandQueue> m_CommandQueue{};

        D3D12_COMMAND_LIST_TYPE m_Type{};

        std::vector<CommandAllocator*> m_Allocators{}; // 执行中的命令分配器

        winrt::com_ptr<ID3D12Fence1> m_Fence{};   // 围栏（用于同步 CPU 和一个或多个 GPU 的对象）
        UINT64 m_FenceValue{ 1 };                    // 围栏值
        HANDLE m_FenceEvent{};
    };
}
