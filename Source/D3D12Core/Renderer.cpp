#include "pch.h"

#include "GraphicsCore.h"
#include "DescriptorHeap.h"
#include "GraphicsBuffer.h"
#include "CommandList.h"
#include "PipelineState.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

#include "Renderer.h"



using namespace std;
using namespace Graphics;


namespace Game
{
    void Renderer::Create(const Mesh* mesh, Material* material, bool useBundle)
    {
        m_BindedMesh = mesh;
        m_BindedMaterial = material;
        m_UseBundle = useBundle;

        if (m_UseBundle)
        {
            m_BundleCommandList.reset(new CommandList());
            m_BundleCommandList->Create(D3D12_COMMAND_LIST_TYPE_BUNDLE);

            if (!m_BindedMaterial->IsChanged())
            {
                RefleshBundleCommandList();
            }
        }
    }

    void Renderer::ExecuteDraw(const Graphics::CommandList* commandList)
    {
        if (m_BindedMaterial->IsChanged())
        {
            // 重新创建管线状态
            m_BindedMaterial->RefleshPipelineState(); // TODO 是否应该在两个不同 PSO 之间切换？而不是重新创建它

            RefleshBundleCommandList();
        }

        if (m_UseBundle)
        {
            // 使用捆绑包绘制
            m_BindedMaterial->ExecuteBindDescriptorHeap(commandList);
            commandList->ExecuteBundle(m_BundleCommandList.get());
        }
        else
        {
            // 不使用捆绑包，直接绘制
            m_BindedMaterial->ExecuteBindMaterial(commandList);
            m_BindedMesh->ExecuteDraw(commandList, m_BindedMaterial->GetShader()->GetBindSemanticFlag());
        }
    }

    void Renderer::RefleshBundleCommandList()
    {
        if (m_UseBundle)
        {
            m_BundleCommandList->Reset();

            m_BindedMaterial->ExecuteBindMaterial(m_BundleCommandList.get());
            m_BindedMesh->ExecuteDraw(m_BundleCommandList.get(), m_BindedMaterial->GetShader()->GetBindSemanticFlag());

            m_BundleCommandList->Close();
        }
    }
}

