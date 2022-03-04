#pragma once

#include "Component.h"

namespace Graphics
{
    class RenderTexture;
    class CommandList;
}

namespace Game
{
    class Mesh;
    class Material;

    class MeshRenderer final : public Component
    {
    public:
        MeshRenderer(GameObject& obj) : Component(obj) {}
        MeshRenderer(const MeshRenderer& mr) = delete;
        MeshRenderer(MeshRenderer&& mr) = default;

        inline MeshRenderer& operator = (const MeshRenderer & mr) = delete;
        inline MeshRenderer& operator = (MeshRenderer && mr) = default;

        void Create(const Mesh* mesh, Material* material, bool useBundle = true);

        void DispatchDraw(const Graphics::CommandList* commandList);

    private:
        bool m_UseBundle; // 是否使用捆绑包命令列表
        std::unique_ptr<Graphics::CommandList> m_BundleCommandList;

        const Mesh* m_BindedMesh;
        Material* m_BindedMaterial;
        UINT64 m_BindedMaterialVersion;

    };

}