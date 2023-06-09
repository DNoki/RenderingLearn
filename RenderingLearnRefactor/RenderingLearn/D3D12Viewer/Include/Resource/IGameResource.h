#pragma once

namespace D3D12Viewer
{
    class IGameResource
    {
    public:
        IGameResource() = default;
        virtual ~IGameResource() = 0 {}
        IGameResource(const IGameResource& buffer) = delete;
        IGameResource(IGameResource&& buffer) = default;
        IGameResource& operator = (const IGameResource& buffer) = delete;
        IGameResource& operator = (IGameResource&& buffer) = default;


        String GetName() const { return m_Name; };
        virtual void SetName(const String& name) = 0;

    protected:
        String m_Name{};

    };
}