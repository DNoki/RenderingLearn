#pragma once

#include "Texture.h"
#include "UploadBuffer.h"

class CommandList;
class GpuPlacedHeap;

class Texture2D : public ITexture
{
public:
    Texture2D() {}

    void DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 1);
    void PlacedCreate(GpuPlacedHeap& pPlacedHeap, DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 1);

    void DispatchCopyTextureData(const CommandList& commandList, const void* data);
    void GenerateChecker(const CommandList& commandList);

private:
    std::unique_ptr<UploadBuffer> m_UploadBuffer;

};
