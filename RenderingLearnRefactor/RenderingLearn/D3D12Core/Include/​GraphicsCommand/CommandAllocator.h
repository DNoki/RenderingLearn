#pragma once

namespace D3D12Core
{
    /**
     * @brief 命令分配器
    */
    class CommandAllocator
    {
    public:
        CommandAllocator() = default;

        void Create(const GraphicsContext& context, D3D12_COMMAND_LIST_TYPE type);

        /**
         * @brief 指示重新使用与命令分配器关联的内存。
        */
        void Reset() const { m_CommandAllocator->Reset(); }

        D3D12_COMMAND_LIST_TYPE GetType() const { return m_Type; }
        ID3D12CommandAllocator* GetD3D12Allocator() const { return m_CommandAllocator.get(); }

        /**
         * \brief 注册当命令完成时的回调
         */
        void AssignOnCompletedCallback(Function<void()> onCompleted);
        /**
         * \brief 通知命令已完成
         */
        void NotifyCompletedEvent();

    private:
        const D3D12_COMMAND_LIST_TYPE m_Type{};
        ComPtr<ID3D12CommandAllocator> m_CommandAllocator{};

        Vector<Function<void()>> m_OnCompletedEvents{};

    };
}
