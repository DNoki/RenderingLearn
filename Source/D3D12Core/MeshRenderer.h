#pragma once

namespace Graphics
{
    class RenderTexture;
    class CommandList;
}

namespace Game
{
    class Mesh;
    class Material;

    class MeshRenderer
    {
    public:
        MeshRenderer() = default;

        void Create(const Mesh* mesh, Material* material, bool useBundle = true);

        void ExecuteDraw(const Graphics::CommandList* commandList);

    private:
        bool m_UseBundle; // 是否使用捆绑包命令列表
        std::unique_ptr<Graphics::CommandList> m_BundleCommandList;

        const Mesh* m_BindedMesh;
        Material* m_BindedMaterial;
        UINT64 m_BindedMaterialVersion;

    };

}