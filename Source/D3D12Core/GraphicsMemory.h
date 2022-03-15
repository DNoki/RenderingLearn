#pragma once

namespace Graphics
{
    class GraphicsResource;

    /**
     * @brief 放置堆
    */
    class PlacedHeap
    {
    public:
        PlacedHeap() = default;

        void Create(D3D12_HEAP_TYPE type, UINT64 size, D3D12_HEAP_FLAGS flags);

        bool PlacedResource(GraphicsResource& resource);
        void ReleaseResource(UINT order);


        inline const D3D12_HEAP_DESC* GetHeapDesc() const { return &m_PlacedHeapDesc; }
        inline UINT64 GetHeapSize() const { return m_PlacedHeapDesc.SizeInBytes; }

        inline ID3D12Heap* GetPlacedHeap() const { return m_PlacedHeap.get(); }

        inline virtual std::wstring GetName() const { return m_Name; }
        inline virtual void SetName(const std::wstring& name)
        {
            m_Name = std::wstring(name);
            if (m_PlacedHeap) SET_DEBUGNAME(m_PlacedHeap.get(), Application::Format(_T("%s (PlacedHeap)"), m_Name.c_str()));
        }

    private:
        winrt::com_ptr<ID3D12Heap> m_PlacedHeap{};    // D3D12 放置堆对象
        D3D12_HEAP_DESC m_PlacedHeapDesc{};           // D3D12 放置堆描述

        UINT m_MinBlockSize{ D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT }; // 最小可分配内存块大小（对齐大小）
        UINT m_MaxOrderSize{}; // 放置堆容量

        std::map<UINT, GraphicsResource*> m_PlacedResources{}; // 已放置的资源
        std::set<UINT> m_MemoryBlockOrders{}; // 已经定位的块索引

        std::wstring m_Name{};
    };

    class GraphicsMemory
    {
    public:
        /**
         * @brief 放置紧凑型资源
         * @param resource
        */
        static void CompactPlacedResource(GraphicsResource& resource); // TODO
        /**
         * @brief 对齐放置资源
         * @param resource
        */
        static void PlacedResource(GraphicsResource& resource);

    private:
        static GraphicsMemory s_GraphicsMemory;
        static GraphicsMemory& GetInstance() { return s_GraphicsMemory; }

        std::vector<std::unique_ptr<PlacedHeap>> m_UploadBufferHeaps{};     // 上传缓冲堆
        std::vector<std::unique_ptr<PlacedHeap>> m_DefaultBufferHeaps{};    // 默认缓冲堆
        std::vector<std::unique_ptr<PlacedHeap>> m_TextureHeaps{};          // 非渲染目标贴图缓冲堆
        std::vector<std::unique_ptr<PlacedHeap>> m_RenderTextureHeaps{};    // 渲染目标贴图缓冲堆
    };
}