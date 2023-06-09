#pragma once

namespace D3D12Core
{
    class ICommandList
    {
    public:
        ICommandList() = default;
        virtual ~ICommandList() = 0 {}
        ICommandList(const ICommandList& buffer) = delete;
        ICommandList(ICommandList&& buffer) = default;
        ICommandList& operator = (const ICommandList& buffer) = delete;
        ICommandList& operator = (ICommandList&& buffer) = default;

        virtual void Close();
        virtual void Reset();

        bool IsLocked() const { return m_IsLocked; }

        D3D12_COMMAND_LIST_TYPE GetType() const { return m_Type; }
        CommandAllocator* GetCommandAllocator() const { return m_CommandAllocator; }

        ID3D12GraphicsCommandList5* GetD3D12CommandList() const { return m_CommandList.get(); }

    public:
        void WriteBufferImmediate();

    protected:
        D3D12_COMMAND_LIST_TYPE m_Type{}; // 命令列表类型
        CommandAllocator* m_CommandAllocator{}; // 命令分配器

        ComPtr<ID3D12GraphicsCommandList5> m_CommandList{};

        /**
         * \brief 命令列表锁定状态
         * true : 列表已关闭，不允许写入命令
         * false : 列表可以写入命令
         */
        bool m_IsLocked{};

    protected:
        void CreateImpl(D3D12_COMMAND_LIST_TYPE type, const String& name);

    };
}
