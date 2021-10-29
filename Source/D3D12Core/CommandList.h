#pragma once

class PipelineState;

class CommandList
{
public:
    CommandList();

    void Create(D3D12_COMMAND_LIST_TYPE type);

    void Reset(const PipelineState* pso);

    inline bool IsLocked() { return m_IsLocked; }
    inline void SetLocked(bool value) { m_IsLocked = value; }

    inline ID3D12GraphicsCommandList5* GetD3D12CommandList() const { return m_CommandList.get(); }
    ID3D12GraphicsCommandList5* operator ->() const { return m_CommandList.get(); }

private:
    D3D12_COMMAND_LIST_TYPE m_Type;
    winrt::com_ptr<ID3D12CommandAllocator> m_CommandAllocator;

    winrt::com_ptr<ID3D12GraphicsCommandList5> m_CommandList;
    //winrt::com_ptr<ID3D12CommandList> m_CommandList;

    bool m_IsLocked;
};
