#pragma once

#include "Texture.h"
#include "UploadBuffer.h"

class CommandList;
class GpuPlacedHeap;

#if 0
class Texture2D : public Texture
{
public:
    Texture2D() {}

    void Create(const Path& path, const DescriptorHandle& pDescriptorHandle);
    void Placed(const Path& path, const DescriptorHandle& pDescriptorHandle, GpuPlacedHeap& pPlacedHeap, GpuPlacedHeap& pUploadPlacedHeap);

    void GenerateChecker(const DescriptorHandle& pDescriptorHandle, UINT width, UINT height);

private:
    std::unique_ptr<UploadBuffer> m_UploadBuffer;
};
#endif // 0

class Texture2D : public ITexture
{
public:
    Texture2D() {}

    void DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 1);
    void PlacedCreate(GpuPlacedHeap& pPlacedHeap, DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 1);

    void CopyTextureData(const CommandList& commandList, const void* data);

private:
    std::unique_ptr<UploadBuffer> m_UploadBuffer;

};
