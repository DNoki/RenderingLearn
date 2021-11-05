#pragma once

/**
 * @brief 命令分配器
*/
class CommandAllocator
{
public:
    CommandAllocator(D3D12_COMMAND_LIST_TYPE type);

    void Restore();

    inline D3D12_COMMAND_LIST_TYPE GetType() { return m_Type; }
    inline ID3D12CommandAllocator* GetD3D12Allocator() { return m_CommandAllocator.get(); }

private:
    const D3D12_COMMAND_LIST_TYPE m_Type;
    winrt::com_ptr<ID3D12CommandAllocator> m_CommandAllocator;
};


/**
 * @brief 命令分配器对象池
*/
class CommandAllocatorPool
{
public:
    static CommandAllocator* Request(D3D12_COMMAND_LIST_TYPE type);
};