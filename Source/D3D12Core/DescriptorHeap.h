#pragma once

#include "DescriptorHandle.h"

namespace Resources
{
    class RenderTargetTexture;
    class DepthStencilTexture;
}

namespace Graphics
{
    class Texture;
    class IBufferResource;

    /**
     * @brief 描述符堆
    */
    class DescriptorHeap
    {
    public:
        DescriptorHeap() = default;

        /**
         * @brief 创建描述符堆
         * @param type 描述符堆类型
         * @param count 描述符数量
        */
        void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count, bool isShaderVisible = false);

        /**
         * @brief 是否是着色器可见
         * @return
        */
        inline bool IsShaderVisible() const { return m_DescriptorHeapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; }

        /**
         * @brief 堆中描述符数量
         * @return
        */
        inline UINT GetDescriptorsCount() const { return m_DescriptorHeapDesc.NumDescriptors; }
        /**
         * @brief 描述符堆类型
         * @return
        */
        inline D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const { return m_DescriptorHeapDesc.Type; }

        /**
         * @brief D3D12 描述符堆
         * @return
        */
        inline ID3D12DescriptorHeap* GetD3D12DescriptorHeap() const { return m_DescriptorHeap.get(); }

        /**
         * @brief 获取描述符句柄
         * @param index
         * @return
        */
        DescriptorHandle GetDescriptorHandle(UINT index) const;

        inline virtual std::wstring GetName() const { return m_Name; }
        inline virtual void SetName(const std::wstring& name)
        {
            m_Name = std::wstring(name);
            if (m_DescriptorHeap) SET_DEBUGNAME(m_DescriptorHeap.get(), Application::Format(_T("%s (DescriptorHeap)"), m_Name.c_str()));
        }

#if 0
        /**
         * @brief 绑定常量缓冲资源
         * @param index
         * @param resource
        */
        void BindConstantBufferView(int index, const IBufferResource& buffer) const;
        /**
         * @brief 绑定纹理资源
         * @param index
         * @param resource
        */
        void BindShaderResourceView(int index, const Texture& tex) const;
        void BindSampler(int index, const D3D12_SAMPLER_DESC& samplerDesc) const;

        void BindRenderTargetView(int index, const Resources::RenderTargetTexture& renderTex) const;
        void BindDepthStencilView(int index, const Resources::DepthStencilTexture& renderTex) const;
#endif

    private:
        winrt::com_ptr<ID3D12DescriptorHeap> m_DescriptorHeap{};    // 描述符堆
        D3D12_DESCRIPTOR_HEAP_DESC m_DescriptorHeapDesc{};          // 描述符堆属性
        UINT m_DescriptorSize{};                    // 单个描述符大小
        DescriptorHandle m_StartDescriptorHandle{}; // 描述符起始句柄

        std::wstring m_Name{};
    };

    class DescriptorAllocator
    {
    public:
        DescriptorAllocator() = delete;

        static DescriptorHandle Allocat(D3D12_DESCRIPTOR_HEAP_TYPE type);

    private:

    };
}
