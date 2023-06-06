#pragma once

namespace D3D12Core
{
    /**
     * @brief 命令分配器
    */
    class CommandAllocator
    {
    public:
        std::vector<std::function<void()>> m_OnCompletedEvents;

        CommandAllocator(D3D12_COMMAND_LIST_TYPE type);

        /**
         * @brief 指示重新使用与命令分配器关联的内存。
        */
        inline void Reset() const { m_CommandAllocator->Reset(); }

        inline D3D12_COMMAND_LIST_TYPE GetType() const { return m_Type; }
        inline ID3D12CommandAllocator* GetD3D12Allocator() { return m_CommandAllocator.get(); }

    private:
        const D3D12_COMMAND_LIST_TYPE m_Type{};
        winrt::com_ptr<ID3D12CommandAllocator> m_CommandAllocator{};
    };
}
