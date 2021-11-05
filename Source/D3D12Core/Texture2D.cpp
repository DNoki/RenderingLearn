﻿#include "pch.h"

//#include <wincodec.h> // TODO WIC库加载图片

#define STB_IMAGE_IMPLEMENTATION //通过定义STB_IMAGE_IMPLEMENTATION，预处理器会修改头文件，让其只包含相关的函数定义源码，等于是将这个头文件变为一个 .cpp 文件了。
#include <stb_image.h>

#include "CommandQueue.h"
#include "GpuPlacedHeap.h"
#include "GraphicsCore.h"
#include "CommandList.h"

#include "Texture2D.h"


using namespace Graphics;


class TextureData
{
public:
    BYTE* Data;
    DXGI_FORMAT Format;
    int Width;
    int Height;

    TextureData(const char* path)
    {
        // 加载并生成纹理
        int nrChannels;

        stbi_set_flip_vertically_on_load(true);// 翻转纹理
        Data = stbi_load(path, &Width, &Height, &nrChannels, 0);

        Format = DXGI_FORMAT_UNKNOWN;
        switch (nrChannels)
        {
        case 1: Format = DXGI_FORMAT_R8_UNORM; break;
        case 2: Format = DXGI_FORMAT_R8G8_UNORM; break;
        case 4: Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
        default: ASSERT(0, L"未定义格式"); break;
        }
    }
    ~TextureData()
    {
        stbi_image_free(Data);
    }

private:

};


void Texture2D::Create(const char* path, const DescriptorHandle& pDescriptorHandle)
{
    // 加载纹理
    auto texData = TextureData(path);
    BYTE* data = texData.Data;
    UINT width = texData.Width, height = texData.Height;
    auto format = texData.Format;


    // 描述并创建2D贴图
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
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
        &m_ResourceDesc,    // 资源的描述
        0,                  // 资源中第一个子资源索引
        1,                  // 资源中子资源数量
        0,                  // 资源的偏移量
        stTxtLayouts,       // 列表，用于填充每个子资源的描述和位置 // RowPitch 内存对齐后的每行大小
        numRows,            // 列表，用于填充每个子资源的行数
        rowSizeInBytes,     // 列表，用于填充每个子资源的每行大小(原始图片行大小)
        &totalBytes);       // 填充总大小
    UINT texturePixelSize = static_cast<UINT>(rowSizeInBytes[0]) / width; // 计算每个像素的字节大小

    // 创建默认堆缓冲
    auto texHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CHECK_HRESULT(g_Device->CreateCommittedResource(
        &texHeapProperties,             // 默认堆类型
        D3D12_HEAP_FLAG_NONE,           // 堆选项
        &m_ResourceDesc,                // 贴图描述
        D3D12_RESOURCE_STATE_COPY_DEST, // 作为GPU复制操作目标，其状态必须为 D3D12_RESOURCE_STATE_COPY_DEST
        nullptr,
        IID_PPV_ARGS(PutD3D12Resource())));

    // 返回要用于数据上传的缓冲区的所需大小
    UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Resource.get(), 0, 1);

    // 创建 GPU 上传缓冲
    m_UploadBuffer = std::unique_ptr<UploadBuffer>(new UploadBuffer());
    m_UploadBuffer->Create(uploadBufferSize);

    // 通过 UpdateSubresources 函数直接通过中间资源将数据拷贝到默认堆
    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = data;
    textureData.RowPitch = width * texturePixelSize;
    textureData.SlicePitch = textureData.RowPitch * height;

    // 使用堆分配实现更新子资源。无需考虑内存对齐
    UpdateSubresources(
        g_GraphicCommandList.GetD3D12CommandList(),  // 命令列表
        m_Resource.get(),                               // 目标资源
        m_UploadBuffer->GetD3D12Resource(),             // 中间资源
        0,                                              // 中间资源的偏移量
        0,                                              // 第一个子资源索引
        1,                                              // 子资源的数目
        &textureData);                                  // 指向数组的指针

    // 资源屏障
    // 转换屏障：描述子资源在不同用途之间的转换，系统将验证命令列表中的子资源转换是否与同一命令列表中以前的转换相一致
    auto barriers = CD3DX12_RESOURCE_BARRIER::Transition(
        m_Resource.get(),
        D3D12_RESOURCE_STATE_COPY_DEST,                 // 之前的状态
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);    // 之后的状态
    g_GraphicCommandList->ResourceBarrier(1, &barriers);


    // 描述并创建纹理的 SRV
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = m_ResourceDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    g_Device->CreateShaderResourceView(m_Resource.get(), &srvDesc, pDescriptorHandle);

    Finalize(&pDescriptorHandle);
}

void Texture2D::Placed(const char* path, const DescriptorHandle& pDescriptorHandle, GpuPlacedHeap& pPlacedHeap, GpuPlacedHeap& pUploadPlacedHeap)
{
    // 加载纹理
    auto texData = TextureData(path);
    BYTE* data = texData.Data;
    UINT width = texData.Width, height = texData.Height;
    auto format = texData.Format;

    // 描述并创建2D贴图
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
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
        &m_ResourceDesc,    // 资源的描述
        0,                  // 资源中第一个子资源索引
        1,                  // 资源中子资源数量
        0,                  // 资源的偏移量
        stTxtLayouts,       // 列表，用于填充每个子资源的描述和位置 // RowPitch 内存对齐后的每行大小
        numRows,            // 列表，用于填充每个子资源的行数
        rowSizeInBytes,     // 列表，用于填充每个子资源的每行大小(原始图片行大小)
        &totalBytes);       // 填充总大小
    UINT texturePixelSize = static_cast<UINT>(rowSizeInBytes[0]) / width; // 计算每个像素的字节大小


    // 使用定位方式创建纹理，这步操作实际上没有实际分配和释放内存，所以性能很高，可以反复调用来创建不同的纹理
    pPlacedHeap.PlacedResource(D3D12_RESOURCE_STATE_COPY_DEST, *this);


    // 返回要用于数据上传的缓冲区的所需大小
    UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Resource.get(), 0, 1);

    // 创建 GPU 上传缓冲
    m_UploadBuffer = std::unique_ptr<UploadBuffer>(new UploadBuffer());
    m_UploadBuffer->Create(uploadBufferSize);
    pUploadPlacedHeap.PlacedResource(D3D12_RESOURCE_STATE_GENERIC_READ, *m_UploadBuffer);


    // 通过 UpdateSubresources 函数直接通过中间资源将数据拷贝到默认堆
    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = data;
    textureData.RowPitch = width * texturePixelSize;
    textureData.SlicePitch = textureData.RowPitch * height;

    // 使用堆分配实现更新子资源。无需考虑内存对齐
    UpdateSubresources(
        g_GraphicCommandList.GetD3D12CommandList(),  // 命令列表
        m_Resource.get(),                               // 目标资源
        m_UploadBuffer->GetD3D12Resource(), // 中间资源，需要注意中间资源的生命周期应当在复制完成之后再释放
        0,                                              // 中间资源的偏移量
        0,                                              // 第一个子资源索引
        1,                                              // 子资源的数目
        &textureData);                                  // 指向数组的指针

    // 资源屏障
    // 转换屏障：描述子资源在不同用途之间的转换，系统将验证命令列表中的子资源转换是否与同一命令列表中以前的转换相一致
    auto barriers = CD3DX12_RESOURCE_BARRIER::Transition(
        m_Resource.get(),
        D3D12_RESOURCE_STATE_COPY_DEST,                 // 之前的状态
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);    // 之后的状态
    g_GraphicCommandList->ResourceBarrier(1, &barriers);


    // 描述并创建纹理的 SRV
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = m_ResourceDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    g_Device->CreateShaderResourceView(m_Resource.get(), &srvDesc, pDescriptorHandle);

    Finalize(&pDescriptorHandle);
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
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
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
        &m_ResourceDesc,    // 资源的描述
        0,                  // 资源中第一个子资源索引
        1,                  // 资源中子资源数量
        0,                  // 资源的偏移量
        stTxtLayouts,       // 列表，用于填充每个子资源的描述和位置 // RowPitch 内存对齐后的每行大小
        numRows,            // 列表，用于填充每个子资源的行数
        rowSizeInBytes,     // 列表，用于填充每个子资源的每行大小(原始图片行大小)
        &totalBytes);       // 填充总大小
    UINT texturePixelSize = static_cast<UINT>(rowSizeInBytes[0]) / width; // 计算每个像素的字节大小

    // 创建默认堆缓冲
    auto texHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CHECK_HRESULT(g_Device->CreateCommittedResource(
        &texHeapProperties,             // 默认堆类型
        D3D12_HEAP_FLAG_NONE,           // 堆选项
        &m_ResourceDesc,                // 贴图描述
        D3D12_RESOURCE_STATE_COPY_DEST, // 作为GPU复制操作目标，其状态必须为 D3D12_RESOURCE_STATE_COPY_DEST
        nullptr,
        IID_PPV_ARGS(PutD3D12Resource())));

    // 关于对齐说明
    //auto 实际行大小 = width * 4;
    //auto 对齐后行大小 = INT_UPPER(width * 4, 256);
    //auto 实际纹理总大小 = 实际行大小 * height;
    //auto 对齐后总大小 = INT_UPPER((height - 1) * 对齐后行大小, 256) + 实际行大小;
    // 返回要用于数据上传的缓冲区的所需大小
    UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Resource.get(), 0, 1);

    // 创建 GPU 上传缓冲
    m_UploadBuffer = std::unique_ptr<UploadBuffer>(new UploadBuffer());
    m_UploadBuffer->Create(uploadBufferSize);

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
    {
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
    }

    // 通过 UpdateSubresources 函数直接通过中间资源将数据拷贝到默认堆
    {
        D3D12_SUBRESOURCE_DATA textureData = {};
        textureData.pData = &texture[0];
        textureData.RowPitch = width * texturePixelSize;
        textureData.SlicePitch = textureData.RowPitch * height;

        // 使用堆分配实现更新子资源。无需考虑内存对齐
        UpdateSubresources(
            g_GraphicCommandList.GetD3D12CommandList(),  // 命令列表
            m_Resource.get(),                               // 目标资源
            m_UploadBuffer->GetD3D12Resource(), // 中间资源，需要注意中间资源的生命周期应当在复制完成之后再释放
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
    g_GraphicCommandList->ResourceBarrier(1, &barriers);


    // 描述并创建纹理的 SRV
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = m_ResourceDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    g_Device->CreateShaderResourceView(m_Resource.get(), &srvDesc, pDescriptorHandle);

    Finalize(&pDescriptorHandle);
}
