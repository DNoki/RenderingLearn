#pragma once

#include "GraphicsResource.h"
#include "UploadBuffer.h"

namespace Graphics
{
    class Texture;
    class CommandList;
}

namespace Game
{

    class Texture2D : public Graphics::Texture
    {
    public:
        Texture2D() {}

        void DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 0);
        void PlacedCreate(DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 0);

        void DispatchCopyTextureData(const Graphics::CommandList& commandList, const void* data);
        void GenerateChecker(const Graphics::CommandList& commandList);

    private:
        std::unique_ptr<Graphics::UploadBuffer> m_UploadBuffer;

    };
}
