#pragma once

#include "GraphicsResource.h"

namespace Graphics
{
    class UploadBuffer;
}

namespace Game
{
    /**
     * @brief 常量缓冲
     * @tparam T
    */
    template<typename T>
    class ConstansBuffer : public IGameResource
    {
    public:
        ConstansBuffer() = default;
        virtual ~ConstansBuffer()
        {
            if (m_BufferResource)
                m_BufferResource->Unmap(0);
        }
        ConstansBuffer(const ConstansBuffer& buffer) = delete;
        ConstansBuffer(ConstansBuffer&& buffer) = default;

        inline ConstansBuffer& operator = (const ConstansBuffer& buffer) = delete;
        inline ConstansBuffer& operator = (ConstansBuffer&& buffer) = default;

        inline void PlacedCreate()
        {
            m_BufferResource.reset(new Graphics::UploadBuffer());

            UINT bufferSize = UINT_UPPER(sizeof(T), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
            m_BufferResource->PlacedCreate(bufferSize);
            m_BufferResource->Map(0, reinterpret_cast<void**>(&m_MappingBuffer));
        }

        inline const Graphics::UploadBuffer* GetResourceBuffer() const { return m_BufferResource.get(); }
        inline T* GetMappingBuffer() const { return m_MappingBuffer; }

        inline virtual std::wstring GetName() const override { return m_Name; }
        inline virtual void SetName(const std::wstring& name) override
        {
            m_Name = std::wstring(name);
            if (m_BufferResource)
                SET_DEBUGNAME(m_BufferResource->GetD3D12Resource(), Application::Format(_T("%s (UploadBuffer)"), m_Name.c_str()));
        }

    private:
        std::unique_ptr<Graphics::UploadBuffer> m_BufferResource{};
        T* m_MappingBuffer{};

        std::wstring m_Name{};

    };
}