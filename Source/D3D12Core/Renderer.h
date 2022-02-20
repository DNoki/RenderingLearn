#pragma once

namespace Graphics
{
    class CommandList;
}

namespace Game
{
    class Renderer
    {
    public:
        Renderer() = default;

    private:
        std::unique_ptr<Graphics::CommandList> m_BundleCommandList;

    };

}