#include "pch.h"
#include "​GraphicsResource/DescriptorHandle.h"

// --------------------------------------------------------------------------
/*
    描述符
    https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/descriptors-overview

    描述符是一个相对较小的数据块，它以特定于 GPU 的不透明格式完全描述到 GPU 的对象。
    有多种不同类型的描述符：
        着色器可见描述符：(DescriptorHandle)
            ・着色器资源视图（SRV）
            ・无序访问视图（UAV）
            ・常量缓冲区视图（CBV）
            ・取样器（Sampler）
        非着色器可见描述符：(DescriptorHandle)
            ・呈现目标视图（RTV）
            ・深度模板视图（DSV）
        直接调用描述符：(D3D12_GPU_VIRTUAL_ADDRESS)
            ・索引缓冲视图（IBV）
            ・顶点缓冲视图（VBV）
            ・流输出视图（SOV）


    描述符句柄是描述符的唯一地址。 它类似于指针，但不透明，因为其实现特定于硬件。
    CPU 句柄可供立即使用，例如，需要同时确定源和目标的复制。
    GPU 句柄不能立即用于 — 标识命令列表中的位置，以便在 GPU 执行时使用。 必须保留它们，直到所有引用它们的命令列表完全执行。


    索引缓冲视图
        填写 D3D12_INDEX_BUFFER_VIEW 结构
        调用 ID3D12GraphicsCommandList::IASetIndexBuffer
    顶点缓冲视图
        填写 D3D12_VERTEX_BUFFER_VIEW 结构
        调用 ID3D12GraphicsCommandList::IASetVertexBuffers
    流输出视图
        填写 D3D12_STREAM_OUTPUT_DESC 结构
        调用 ID3D12GraphicsCommandList::SOSetTargets

    着色器资源视图
        填写 D3D12_SHADER_RESOURCE_VIEW_DESC 结构
        调用 ID3D12Device::CreateShaderResourceView
    无序访问视图
        填写 D3D12_UNORDERED_ACCESS_VIEW_DESC 结构
        调用 ID3D12Device::CreateUnorderedAccessView
    常量缓冲区视图
        填写 D3D12_CONSTANT_BUFFER_VIEW_DESC 结构
        调用 ID3D12Device::CreateConstantBufferView
    取样器
        填写 D3D12_SAMPLER_DESC 结构
        调用 ID3D12Device::CreateSampler

    呈现目标视图
        填写 D3D12_RENDER_TARGET_VIEW_DESC 结构
        调用 ID3D12Device::CreateRenderTargetView
    深度模板视图
        填写 D3D12_DEPTH_STENCIL_VIEW_DESC 结构
        调用 ID3D12Device::CreateDepthStencilView


*/
// --------------------------------------------------------------------------

using namespace D3D12Core;

const DescriptorHandle DescriptorHandle::DESCRIPTOR_HANDLE_NULL = DescriptorHandle
{
    CpuDescriptorHandle{D3D12_GPU_VIRTUAL_ADDRESS_NULL},
    GpuDescriptorHandle{D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN}
};
