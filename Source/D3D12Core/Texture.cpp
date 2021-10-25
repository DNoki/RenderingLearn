#include "pch.h"

#include <wincodec.h> // TODO WIC库加载图片

#include "GraphicsCore.h"
#include "Display.h"
#include "CommandQueue.h"

#include "Texture.h"

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
*/
// --------------------------------------------------------------------------

// 边界对齐，B为2的指数倍
#define INT_UPPER(A, B) (((UINT)A + B - 1)&~(B - 1))

using namespace Graphics;


RenderTexture::RenderTexture() : m_RtvDesc(nullptr)
{
    //ZeroMemory(this, sizeof(*this));
}

void RenderTexture::Create(const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, const DescriptorHandle& pDescriptorHandle, UINT width, UINT height)
{
    ASSERT(m_Resource != nullptr);

    SetAddressOrDescriptor(D3D12_GPU_VIRTUAL_ADDRESS_NULL, pDescriptorHandle);

    auto pRtvDesc = pDesc ? new D3D12_RENDER_TARGET_VIEW_DESC(*pDesc) : nullptr;
    m_RtvDesc = std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC>(pRtvDesc);

    g_Device->CreateRenderTargetView(m_Resource.get(), m_RtvDesc.get(), m_DescriptorHandle);

    m_Width = width;
    m_Height = height;
}

void RenderTexture::CreateFromSwapChain(UINT index, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, const DescriptorHandle& pDescriptorHandle)
{
    ASSERT(m_Resource == nullptr);

    CHECK_HRESULT(g_SwapChain->GetBuffer(index, IID_PPV_ARGS(m_Resource.put())));

    Create(pDesc, pDescriptorHandle, Display::GetScreenWidth(), Display::GetScreenHeight());
}


void Texture2D::GenerateChecker(const DescriptorHandle& pDescriptorHandle, UINT width, UINT height)
{
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

    //D3D12_RESOURCE_DESC textureDesc = {};
    //textureDesc.MipLevels = 1;
    //textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //textureDesc.Width = width;
    //textureDesc.Height = height;
    //textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    //textureDesc.DepthOrArraySize = 1;
    //textureDesc.SampleDesc.Count = 1;
    //textureDesc.SampleDesc.Quality = 0;
    //textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    // 描述并创建2D贴图
    auto textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        format,
        width,
        height,
        1, 1
        // 1, 0, D3D12_RESOURCE_FLAG_NONE, D3D12_TEXTURE_LAYOUT_UNKNOWN, 0
    );

    // 使用 GetCopyableFootprints 函数来获取可复制资源布局
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT stTxtLayouts[1] = {};
    UINT numRows[1] = {};
    UINT64 rowSizeInBytes[1] = {};
    UINT64 totalBytes = 0;
    g_Device->GetCopyableFootprints(
        &textureDesc,       // 资源的描述
        0,                  // 资源中第一个子资源索引
        1,                  // 资源中子资源数量
        0,                  // 资源的偏移量
        stTxtLayouts,       // 列表，用于填充每个子资源的描述和位置 // RowPitch 内存对齐后的每行大小
        numRows,            // 列表，用于填充每个子资源的行数
        rowSizeInBytes,     // 列表，用于填充每个子资源的每行大小(原始图片行大小)
        &totalBytes);       // 填充总大小
    UINT texturePixelSize = rowSizeInBytes[0] / width; // 计算每个像素的字节大小

    // 创建默认堆缓冲
    auto texHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CHECK_HRESULT(g_Device->CreateCommittedResource(
        &texHeapProperties,             // 默认堆类型
        D3D12_HEAP_FLAG_NONE,           // 堆选项
        &textureDesc,                   // 贴图描述
        D3D12_RESOURCE_STATE_COPY_DEST, // 作为GPU复制操作目标，其状态必须为 D3D12_RESOURCE_STATE_COPY_DEST
        nullptr,
        IID_PPV_ARGS(m_Resource.put())));

    // 关于对齐说明
    //auto 实际行大小 = width * 4;
    //auto 对齐后行大小 = INT_UPPER(width * 4, 256);
    //auto 实际纹理总大小 = 实际行大小 * height;
    //auto 对齐后总大小 = INT_UPPER((height - 1) * 对齐后行大小, 256) + 实际行大小;
    // 返回要用于数据上传的缓冲区的所需大小
    UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Resource.get(), 0, 1);

    // 创建 GPU 上传缓冲
    auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
    CHECK_HRESULT(g_Device->CreateCommittedResource(
        &uploadHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &buffer,
        D3D12_RESOURCE_STATE_GENERIC_READ, // 上传堆的初始状态必须是 D3D12_RESOURCE_STATE_GENERIC_READ ，且不能更改
        nullptr,
        IID_PPV_ARGS(m_UploadResource.put()))); // 注意：上传资源的生命周期必须等待GPU复制完成之后才能释放

    // 生成 Checker 贴图
    std::vector<UINT8> texture;
    {
        const UINT rowPitch = width * texturePixelSize;
        const UINT cellPitch = rowPitch >> 3;   // The width of a cell in the checkboard texture.
        const UINT cellHeight = width >> 3;     // The height of a cell in the checkerboard texture.
        const UINT textureSize = rowPitch * height;

        texture = std::vector<UINT8>(textureSize);
        UINT8* pData = &texture[0];

        for (UINT n = 0; n < textureSize; n += texturePixelSize)
        {
            UINT x = n % rowPitch;
            UINT y = n / rowPitch;
            UINT i = x / cellPitch;
            UINT j = y / cellHeight;

            if (i % 2 == j % 2)
            {
                pData[n] = 0x00;        // R
                pData[n + 1] = 0x00;    // G
                pData[n + 2] = 0x00;    // B
                pData[n + 3] = 0xff;    // A
            }
            else
            {
                pData[n] = 0xff;        // R
                pData[n + 1] = 0xff;    // G
                pData[n + 2] = 0xff;    // B
                pData[n + 3] = 0xff;    // A
            }
        }
    }

    // 先拷贝资源到上传堆，在添加拷贝命令到GPU
    //{
    //    // 使用Map、memcpy、Unmap方式向上传堆拷贝数据
    //    UINT8* pData = nullptr;
    //    m_UploadResource->Map(0, NULL, reinterpret_cast<void**>(&pData));

    //    // 这里实际使用按行拷贝的方式，因为实际图片行大小与对齐大小可能不一致
    //    UINT8* pDstSliceData = pData + stTxtLayouts->Offset; // 计算子资源偏移量
    //    UINT8* pSrcSlice = texture.data();
    //    for (UINT i = 0; i < numRows[0]; i++)
    //    {
    //        memcpy(
    //            pDstSliceData + stTxtLayouts->Footprint.RowPitch * i,
    //            pSrcSlice + rowSizeInBytes[0] * i,
    //            rowSizeInBytes[0]);
    //    }

    //    m_UploadResource->Unmap(0, NULL);

    //    //向命令队列发出从上传堆复制纹理数据到默认堆的命令
    //    D3D12_TEXTURE_COPY_LOCATION stDst = {};
    //    stDst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    //    stDst.pResource = m_Resource.get();
    //    stDst.SubresourceIndex = 0;

    //    D3D12_TEXTURE_COPY_LOCATION stSrc = {};
    //    stSrc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    //    stSrc.pResource = m_UploadResource.get();
    //    stSrc.PlacedFootprint = stTxtLayouts[0];

    //    // 添加复制命令到队列
    //    g_GraphicsCommandQueue->GetD3D12CommandList()->CopyTextureRegion(&stDst, 0, 0, 0, &stSrc, nullptr);
    //}

    // 通过 UpdateSubresources 函数直接通过中间资源将数据拷贝到默认堆
    {
        D3D12_SUBRESOURCE_DATA textureData = {};
        textureData.pData = &texture[0];
        textureData.RowPitch = width * texturePixelSize;
        textureData.SlicePitch = textureData.RowPitch * height;

        // 使用堆分配实现更新子资源。无需考虑内存对齐
        UpdateSubresources(
            g_GraphicsCommandQueue->GetD3D12CommandList(),  // 命令列表
            m_Resource.get(),                               // 目标资源
            m_UploadResource.get(), // 中间资源，需要注意中间资源的生命周期应当在复制完成之后再释放
            0,                                              // 中间资源的偏移量
            0,                                              // 第一个子资源索引
            1,                                              // 子资源的数目
            &textureData);                                  // 指向数组的指针
    }


    // 资源屏障
    // 转换屏障：描述子资源在不同用途之间的转换，系统将验证命令列表中的子资源转换是否与同一命令列表中以前的转换相一致
    auto barriers = CD3DX12_RESOURCE_BARRIER::Transition(
        m_Resource.get(),
        D3D12_RESOURCE_STATE_COPY_DEST,                 // 之前的状态
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);    // 之后的状态
    g_GraphicsCommandQueue->GetD3D12CommandList()->ResourceBarrier(1, &barriers);


    // 描述并创建纹理的 SRV
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    g_Device->CreateShaderResourceView(m_Resource.get(), &srvDesc, pDescriptorHandle);
}
