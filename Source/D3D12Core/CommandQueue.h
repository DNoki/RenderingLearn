#pragma once

class PipelineState;

/**
 * @brief 指令队列
*/
class CommandQueue
{
public:
    CommandQueue();

    /**
     * @brief 创建指令队列
     * @param type 指令列表类型
    */
    void Create(D3D12_COMMAND_LIST_TYPE type);
    /**
     * @brief 创建指令列表
     * @param pso
    */
    void CreateCommandList(PipelineState* pso);

    /**
     * @brief 等待队列完成
    */
    void WaitForQueueCompleted();

    /**
     * @brief 关闭指令队列并释放资源
    */
    void CloseQueue();

    inline ID3D12CommandQueue* GetD3D12CommandQueue() const { return m_CommandQueue.get(); }
    inline ID3D12CommandAllocator* GetD3D12CommandAllocator() const { return m_CommandAllocator.get(); }
    inline ID3D12GraphicsCommandList5* GetD3D12CommandList() const { return m_CommandList.get(); }

private:
    winrt::com_ptr<ID3D12CommandQueue> m_CommandQueue;
    winrt::com_ptr<ID3D12CommandAllocator> m_CommandAllocator;
    winrt::com_ptr<ID3D12GraphicsCommandList5> m_CommandList; // TODO 解耦命令队列

    D3D12_COMMAND_LIST_TYPE m_Type;

    winrt::com_ptr<ID3D12Fence1> m_Fence;   // 围栏（用于同步 CPU 和一个或多个 GPU 的对象）
    UINT64 m_FenceValue;                    // 围栏值
    HANDLE m_FenceEvent;

    bool m_IsClose;
};