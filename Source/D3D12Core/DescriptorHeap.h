#pragma once

#include "DescriptorHandle.h"

class IBufferResource;
class ITexture;
class RenderTexture;

/**
 * @brief 描述符堆
*/
class DescriptorHeap
{
public:
    DescriptorHeap();

    /**
     * @brief 创建描述符堆
     * @param type 描述符堆类型
     * @param count 描述符数量
    */
    void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count);

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

    /**
     * @brief 绑定常量缓冲资源
     * @param index
     * @param resource
    */
    void BindConstantBufferView(int index, const IBufferResource& buffer);
    /**
     * @brief 绑定纹理资源
     * @param index
     * @param resource
    */
    void BindShaderResourceView(int index, const ITexture& tex);

    void BindRenderTargetView(int index, const RenderTexture& renderTex);

private:
    winrt::com_ptr<ID3D12DescriptorHeap> m_DescriptorHeap;        // 描述符堆
    D3D12_DESCRIPTOR_HEAP_DESC m_DescriptorHeapDesc;              // 描述符堆属性
    UINT m_DescriptorSize;                              // 单个描述符大小
    DescriptorHandle m_StartDescriptorHandle;           // 描述符起始句柄

    /**
     * @brief 单个描述符大小
     * @return
    */
    inline UINT GetDescriptorSize() const { return m_DescriptorSize; }
};


// 这是一个无界资源描述符分配器。 它旨在在创建资源时为 CPU 可见的资源描述符提供空间。 对于需要使着色器可见的那些，需要将它们复制到 DescriptorHeap 或 DynamicDescriptorHeap。
// class DescriptorAllocator;


// 这个类是动态生成的描述符表的线性分配系统。 它在内部缓存 CPU 描述符句柄，以便当当前堆中没有足够的可用空间时，可以将必要的描述符重新复制到新堆中。
// class DynamicDescriptorHeap
