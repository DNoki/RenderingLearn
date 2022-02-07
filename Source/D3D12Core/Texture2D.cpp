#include "pch.h"

#include "GpuPlacedHeap.h"
#include "GraphicsCore.h"
#include "CommandList.h"

#include "Texture2D.h"


using namespace std;
using namespace Graphics;


void Texture2D::DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize, UINT16 mipLevels)
{
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 1);

    // 纹理资源的存储堆类型必须为 默认堆
    // 无法在 D3D12_HEAP_TYPE_UPLOAD 或 D3D12_HEAP_TYPE_READBACK 堆上创建纹理资源。
    // 研究 CopyTextureRegion 以在 CPU 可访问缓冲区中复制纹理数据，或研究 D3D12_HEAP_TYPE_CUSTOM 和 WriteToSubresource 以优化 UMA 适配器。
    auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CHECK_HRESULT(g_Device->CreateCommittedResource(
        &heapType,             // 默认堆类型
        D3D12_HEAP_FLAG_NONE,           // 堆选项
        &m_ResourceDesc,                // 贴图描述
        D3D12_RESOURCE_STATE_COPY_DEST, // 作为GPU复制操作目标，其状态必须为 D3D12_RESOURCE_STATE_COPY_DEST
        nullptr,
        IID_PPV_ARGS(PutD3D12Resource())));
}

void Texture2D::PlacedCreate(GpuPlacedHeap& pPlacedHeap, DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize, UINT16 mipLevels)
{
    m_ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 1);

    // 要放入的放置堆类型必须是上传堆
    ASSERT(pPlacedHeap.GetHeapDesc()->Properties.Type == D3D12_HEAP_TYPE_DEFAULT);

    pPlacedHeap.PlacedResource(D3D12_RESOURCE_STATE_COPY_DEST, *this);
}

void Texture2D::DispatchCopyTextureData(const CommandList& commandList, const void* data)
{
    ASSERT(m_Resource != nullptr);

    // 创建中间缓冲
    {
        // 关于对齐说明
        //auto 实际行大小 = width * 4;
        //auto 对齐后行大小 = INT_UPPER(width * 4, 256);
        //auto 实际纹理总大小 = 实际行大小 * height;
        //auto 对齐后总大小 = INT_UPPER((height - 1) * 对齐后行大小, 256) + 实际行大小;
        // 返回要用于数据上传的缓冲区的所需大小
        UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Resource.get(), 0, 1);
        // 创建 GPU 上传缓冲
        m_UploadBuffer = std::unique_ptr<UploadBuffer>(new UploadBuffer());
        m_UploadBuffer->DirectCreate(uploadBufferSize);
    }

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
    auto texturePixelSize = static_cast<UINT>(rowSizeInBytes[0] / m_ResourceDesc.Width); // 计算每个像素的字节大小

    // 拷贝纹理方法一：
    //  使用Map、memcpy、Unmap方式向上传堆拷贝数据，再使用 CopyTextureRegion 函数复制到默认堆
    {
#if 0
        UINT8* pData = nullptr;
        m_UploadBuffer->Map(0, reinterpret_cast<void**>(&pData));

        // 这里实际使用按行拷贝的方式，因为实际图片行大小与对齐大小可能不一致
        UINT8* pDstSliceData = pData + stTxtLayouts->Offset; // 计算子资源偏移量
        const UINT8* pSrcSlice = reinterpret_cast<const UINT8*>(data);
        for (UINT i = 0; i < numRows[0]; i++)
        {
            memcpy(pDstSliceData + stTxtLayouts->Footprint.RowPitch * i,
                pSrcSlice + rowSizeInBytes[0] * i,
                rowSizeInBytes[0]);
        }

        m_UploadBuffer->Unmap(0);

        //向命令队列发出从上传堆复制纹理数据到默认堆的命令
        D3D12_TEXTURE_COPY_LOCATION stDst = {};
        stDst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        stDst.pResource = m_Resource.get();
        stDst.SubresourceIndex = 0;

        D3D12_TEXTURE_COPY_LOCATION stSrc = {};
        stSrc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        stSrc.pResource = m_UploadBuffer->GetD3D12Resource();
        stSrc.PlacedFootprint = stTxtLayouts[0];

        // 添加复制命令到队列
        commandList->CopyTextureRegion(&stDst, 0, 0, 0, &stSrc, nullptr);
#endif
    }

    // 拷贝纹理方法二：
    //  通过 UpdateSubresources 函数直接通过中间资源将数据拷贝到默认堆
    {
        D3D12_SUBRESOURCE_DATA textureData = {};
        textureData.pData = data;
        textureData.RowPitch = m_ResourceDesc.Width * texturePixelSize;
        textureData.SlicePitch = textureData.RowPitch * m_ResourceDesc.Height;

        // 使用堆分配实现更新子资源。无需考虑内存对齐
        UpdateSubresources(
            commandList.GetD3D12CommandList(),  // 命令列表
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
        // 之后的状态
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE // 该资源与像素着色器以外的着色器一起使用。
        | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE // 该资源与像素着色器一起使用。
    );
    commandList->ResourceBarrier(1, &barriers);
}

void Texture2D::GenerateChecker(const CommandList& commandList)
{
    ASSERT(m_Resource != nullptr);

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
    auto texturePixelSize = static_cast<UINT>(rowSizeInBytes[0] / m_ResourceDesc.Width); // 计算每个像素的字节大小

    // 生成 Checker 贴图
    std::vector<UINT8> texture;
    {
        auto width = static_cast<UINT>(m_ResourceDesc.Width);
        auto height = static_cast<UINT>(m_ResourceDesc.Height);
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

    DispatchCopyTextureData(commandList, texture.data());
}
