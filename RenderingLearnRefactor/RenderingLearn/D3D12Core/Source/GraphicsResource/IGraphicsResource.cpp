#include "pch.h"
#include "​GraphicsResource/IGraphicsResource.h"

//#include "GraphicsMemory.h"
//#include "CommandList.h"
//
//#include "GraphicsResource.h"

// --------------------------------------------------------------------------
/*
    D3D12 中的内存管理
    https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/memory-management

    CPU+GPU架构中，主要有三种内存管理方式
        ·CPU 独占内存
        ·CPU、GPU 共享内存（通过总线连接）
        ·GPU 独占内存

    D3D12 中主要有四种资源类型
        ·默认堆
            位于GPU独占内存，由于CPU无法访问，需要通过上传堆+复制命令来向其拷贝数据，一般用于存放不易改变的纹理之类的数据
        ·上传堆
            位于共享内存中，一般对于CPU来说是只写，对于GPU来说是只读。一般用于存放经常改变的数据，如世界变换矩阵、动画矩阵调色板等
            因为共享内存CPU访问相对简单，可以直接Map、memcpy、Unmap来更改其数据
        ·回读堆
            用于GPU写入然后CPU读取类型数据的。通常用于多趟渲染中的流输出数据等，当然有时也用于读取离屏渲染画面数据的时候。对于CPU来说是只读，对于GPU来说是只写。并且往往需要向GPU标识其为UAV（Unorder Access View 无序访问视图）形式的数据。
        ·自定义堆
            提供了自由组合CPU和GPU访问方式的可能，同时还可以指定它在共享内存中，还是在显存中。


    关于边界对齐
        在 D3D12 中根据现代 GPU 访问存储的边界对齐要求，纹理的行大小必须是256字节边界对齐，整个纹理的大小必须是512（实测为256字节）字节边界对齐。
        例如：600x600的纹理，每个像素有4字节（R8G8B8A8）大小，
            每行应当对齐到
                256 * [(600 * 4 + 256 - 1) / 256] = 2560
            总大小应当对齐到（实测为256字节）
                256 * [(2560 * (600 - 1) + 256 - 1) / 256] + (600 * 4) = 1535840
                ﹡这里最后一行不需要补全字节所以直接添加实际位数
            [(每行大小2400)(补齐大小160)][(每行大小2400)(补齐大小160)]……[(每行大小2400)]
        ﹡上取整算法①：(A+B-1)/B
        ﹡上取整算法②：(A+B-1)&~(B-1)  // 边界对齐，B为2的指数倍


    关于资源（ID3D12Resource）
        两种常见的资源是缓冲区和纹理。
            缓冲区
                缓冲区是一个连续的内存区域。对齐必须是 64KB。
                Height、DepthOrArraySize和MipLevels必须为 1。
                格式必须是 DXGI_FORMAT_UNKNOWN。
                SampleDesc.Count必须为 1，Quality必须为 0。
                布局必须是 D3D12_TEXTURE_LAYOUT_ROW_MAJOR，因为应用程序可以理解缓冲区内存布局，并且行主纹理数据通常通过缓冲区进行编组。
            纹理
                纹理是连续内存区域中纹素的多维排列，经过大量优化以最大化渲染和采样的带宽。纹理大小难以预测，并且因适配器而异。应用程序必须使用ID3D12Device::GetResourceAllocationInfo来准确了解它们的大小。
                Width、Height和DepthOrArraySize必须介于 1 和特定特征级别和纹理维度支持的最大维度之间。
                对齐可以是 0、4KB、64KB 或 4MB 之一。
                    如果Alignment设置为 0，运行时将使用 4MB 用于 MSAA 纹理，64KB 用于其他所有纹理。当纹理较小时，应用程序可能会为几种纹理类型选择比这些默认值更小的对齐方式。
                MipLevels可以是 0 或 1 到Width、Height和DepthOrArraySize维度支持的最大 mip 级别。当使用 0 时，API 将自动计算支持的最大 mip 级别并使用它。但是，某些资源和堆属性排除了 mip 级别，因此应用程序必须将该值指定为 1。
        创建资源
            在 D3D12 中有三种方式创建资源
                提交方式（ID3D12Device::CreateCommittedResource）
                    直接在内存中分配一块指定大小的资源
                    传统的创建方法，在 D3D12 中不推荐使用
                定位方式（ID3D12Device::CreatePlacedResource）
                    定位方式是 D3D12 中新增的方法，需要先使用 ID3D12Device::CreateHeap 方法来申请一大块内存堆，然后再将资源放置在申请的堆中
                保留方式（ID3D12Device::CreateReservedResource）


    关于子资源（Subresouorces）
        若某一资源包含缓冲区，则它仅包含索引编号为0的一个子资源。
        若一个资源是纹理资源，则引用子资源更为复杂（MipMap）。
        https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/subresources
*/
// --------------------------------------------------------------------------

void D3D12Core::IGraphicsResource::DispatchTransitionStates(const GraphicsCommandList* commandList,
    D3D12_RESOURCE_STATES after)

{
    ASSERT(m_ResourceStates != after);
    commandList->ResourceTransitionBarrier(this, m_ResourceStates, after);
    m_ResourceStates = after; // TODO 资源状态并非是立即更新的
}