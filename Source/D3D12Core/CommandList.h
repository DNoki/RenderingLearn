#pragma once

namespace Graphics
{
    class CommandAllocator;
    class PipelineState;

    class CommandList
    {
    public:
        CommandList();
        ~CommandList();

        /**
         * @brief 创建命令队列
         * @param type 命令队列类型
         * @param isAllocate 是否分配内存
         * @param pso 初始管线状态
        */
        void Create(D3D12_COMMAND_LIST_TYPE type, bool isAllocate = false, const PipelineState* pso = nullptr);

        void Close();
        void Reset(const PipelineState* pso);

        inline bool IsLocked() const { return m_IsLocked; }

        inline D3D12_COMMAND_LIST_TYPE GetType() const { return m_Type; }
        inline CommandAllocator* GetCommandAllocator() const { return m_CommandAllocator; }
        inline ID3D12GraphicsCommandList5* GetD3D12CommandList() const { return m_CommandList.get(); }

    private:
        D3D12_COMMAND_LIST_TYPE m_Type;
        CommandAllocator* m_CommandAllocator;

        winrt::com_ptr<ID3D12GraphicsCommandList5> m_CommandList;
        //winrt::com_ptr<ID3D12CommandList> m_CommandList;

        bool m_IsLocked; // 是否允许写入命令
    };
}
