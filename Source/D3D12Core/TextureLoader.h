#pragma once

namespace Application
{
    class TextureLoader
    {
    public:
        TextureLoader() = default;

        void LoadTexture2D(const Path& path);

        inline const UINT8* GetDataPointer() const { return m_Data.data(); }
        inline DXGI_FORMAT GetFormat() const { return m_Format; }
        inline int GetWidth() const { return m_Width; }
        inline int GetHeight() const { return m_Height; }

    private:
        std::vector<UINT8> m_Data{};
        DXGI_FORMAT m_Format{};
        int m_Width{};
        int m_Height{};
    };
}
