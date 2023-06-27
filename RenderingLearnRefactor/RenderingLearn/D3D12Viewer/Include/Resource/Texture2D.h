#pragma once

#include "​GraphicsResource/ITexture.h"

namespace D3D12Viewer
{
    class Texture2D final : public D3D12Core::ITexture, public IGameResource
    {
    public:
        Texture2D() = default;
        virtual ~Texture2D() override = default;

        void Create(DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 0);

        void DispatchCopyTextureData(const void* data);
        //void DispatchCopyTextureData(const Graphics::CommandList& commandList, const void* data);
        //void GenerateChecker(const Graphics::CommandList& commandList);

        //inline virtual std::wstring GetName() const override { return Texture::GetName(); }
        void SetName(const String& name) override
        {
            m_Name = name;
            if (m_Resource)
            {
                D3D12Core::GraphicsContext::SetDebugName(m_Resource.get(), FORMAT(TEXT("%s (Texture2D)"), m_Name.c_str()));
            }
        }

    private:
        void InitDescriptor();

    };
}
