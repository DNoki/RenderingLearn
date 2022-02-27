#pragma once

#include "UploadBuffer.h"

namespace Graphics
{
    class CommandList;
    class GpuPlacedHeap;

    class Texture2D : public ITexture
    {
    public:
        Texture2D() {}

        void DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 0);
        void PlacedCreate(DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 0);

        void DispatchCopyTextureData(const CommandList& commandList, const void* data);
        void GenerateChecker(const CommandList& commandList);

    private:
        std::unique_ptr<UploadBuffer> m_UploadBuffer;

    };
}
