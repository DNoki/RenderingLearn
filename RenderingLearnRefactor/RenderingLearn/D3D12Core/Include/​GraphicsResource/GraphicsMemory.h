#pragma once

namespace D3D12Core
{
    class PlacedHeap;

    class GraphicsMemory
    {
    public:
        /**
         * @brief 放置紧凑型资源
         * @param resource
        */
        static void CompactPlacedResource(IGraphicsResource& resource); // TODO
        /**
         * @brief 对齐放置资源
         * @param resource
        */
        static void PlacedResource(IGraphicsResource& resource);

    private:
        inline static UniquePtr<GraphicsMemory> s_GraphicsMemory = nullptr;
        static GraphicsMemory& GetInstance()
        {
            if (!s_GraphicsMemory)
                s_GraphicsMemory = std::make_unique<GraphicsMemory>();
            return *s_GraphicsMemory;
        }

        Vector<UniquePtr<PlacedHeap>> m_UploadBufferHeaps{};     // 上传缓冲堆
        Vector<UniquePtr<PlacedHeap>> m_DefaultBufferHeaps{};    // 默认缓冲堆
        Vector<UniquePtr<PlacedHeap>> m_TextureHeaps{};          // 非渲染目标贴图缓冲堆
        Vector<UniquePtr<PlacedHeap>> m_RenderTextureHeaps{};    // 渲染目标贴图缓冲堆
    };
}