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

    class Texture2D final : public Graphics::Texture, public IGameResource
    {
    public:
        Texture2D() {}
        virtual ~Texture2D() override = default;

        void DirectCreate(DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 0);
        void PlacedCreate(DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 0);

        void DispatchCopyTextureData(const Graphics::CommandList& commandList, const void* data);
        void GenerateChecker(const Graphics::CommandList& commandList);

        inline virtual std::wstring GetName() const override { return Texture::GetName(); }
        inline virtual void SetName(const std::wstring& name) override
        {
            m_Name = std::wstring(name);
            if (m_Resource) SET_DEBUGNAME(m_Resource.get(), Application::Format(_T("%s (Texture2D)"), m_Name.c_str()));
        }

    private:
        std::unique_ptr<Graphics::UploadBuffer> m_UploadBuffer;

    };
}
