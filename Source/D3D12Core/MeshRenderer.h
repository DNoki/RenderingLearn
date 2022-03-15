﻿#pragma once

#include "Component.h"

namespace Resources
{
    class RenderTexture;
}

namespace Graphics
{
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
        virtual ~MeshRenderer() override = default;
        MeshRenderer(const MeshRenderer& mr) = delete;
        MeshRenderer(MeshRenderer&& mr) = default;

        inline MeshRenderer& operator = (const MeshRenderer& mr) = delete;
        inline MeshRenderer& operator = (MeshRenderer&& mr) = default;

        inline const Mesh* GetMesh() const { return m_BindedMesh; }
        inline Material* GetMaterial() const { return m_BindedMaterial; }

        void BindResource(const Mesh* mesh, Material* material, bool useBundle = true);

        void DispatchDraw(const Graphics::CommandList* commandList);

    private:
        bool m_UseBundle{ false }; // 是否使用捆绑包命令列表
        std::unique_ptr<Graphics::CommandList> m_BundleCommandList{};

        const Mesh* m_BindedMesh{};
        Material* m_BindedMaterial{};
        UINT64 m_BindedMaterialVersion{ 0 };

    };

}