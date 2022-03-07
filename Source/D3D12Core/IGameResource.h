#pragma once

namespace Game
{
    class IGameResource
    {
    public:
        IGameResource() = default;
        virtual ~IGameResource() = 0 {}

        virtual std::wstring GetName() const = 0;
        virtual void SetName(const std::wstring& name) = 0;

    private:

    };
}