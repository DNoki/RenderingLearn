#pragma once

class CommandAllocator;
class PipelineState;

class CommandList
{
public:
    CommandList();

    void Create(D3D12_COMMAND_LIST_TYPE type);

    void Reset(const PipelineState* pso);

    inline bool IsLocked() { return m_IsLocked; }
    inline void SetLocked()
    {
        m_IsLocked = true;
        m_CommandAllocator = nullptr; // 命令列表在执行后可以立即重置，这里取消引用之前的分配器
    }
    inline CommandAllocator* GetCommandAllocator() { return m_CommandAllocator; }

    inline ID3D12GraphicsCommandList5* GetD3D12CommandList() const { return m_CommandList.get(); }
    ID3D12GraphicsCommandList5* operator ->() const { return m_CommandList.get(); }

private:
    D3D12_COMMAND_LIST_TYPE m_Type;
    CommandAllocator* m_CommandAllocator;

    winrt::com_ptr<ID3D12GraphicsCommandList5> m_CommandList;
    //winrt::com_ptr<ID3D12CommandList> m_CommandList;

    bool m_IsLocked;
};
