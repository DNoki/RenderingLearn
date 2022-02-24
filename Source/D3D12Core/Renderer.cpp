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
        m_BindedMaterialVersion = 0;
        m_UseBundle = useBundle;

        m_BundleCommandList = nullptr;
        if (m_UseBundle)
        {
            m_BundleCommandList.reset(new CommandList());
            m_BundleCommandList->Create(D3D12_COMMAND_LIST_TYPE_BUNDLE);
        }
    }

    void Renderer::ExecuteDraw(const Graphics::CommandList* commandList)
    {
        // 必须由图形命令列表调用
        ASSERT(commandList->GetType() == D3D12_COMMAND_LIST_TYPE_DIRECT);

        // 设置当前渲染目标格式
        m_BindedMaterial->SetRenderTargetsFormat(commandList->GetRenderTargetInfos());

        // 检测管线状态是否已更改
        bool isRefleshBundle = false;
        if (m_BindedMaterial->GetPipelineState()->CheckStateChanged())
        {
            // 重新创建管线状态
            m_BindedMaterial->GetPipelineState()->Finalize();
            isRefleshBundle = true;
        }

        // 检测材质绑定资源是否已更改
        if ((m_BindedMaterial->GetVersion() > m_BindedMaterialVersion) || isRefleshBundle)
        {
            RefleshBundleCommandList(); // 重新捆绑命令列表
            m_BindedMaterialVersion = m_BindedMaterial->GetVersion();
        }

        if (m_UseBundle)
        {
            // 使用捆绑包绘制
            m_BindedMaterial->ExecuteBindMaterial(commandList, true);
            commandList->ExecuteBundle(m_BundleCommandList.get());
        }
        else
        {
            // 不使用捆绑包，直接绘制
            m_BindedMaterial->ExecuteBindMaterial(commandList, false);
            m_BindedMesh->ExecuteDraw(commandList, m_BindedMaterial->GetShader()->GetBindSemanticFlag());
        }
    }

    void Renderer::RefleshBundleCommandList()
    {
        if (m_UseBundle)
        {
            m_BundleCommandList->Reset();

            m_BindedMaterial->ExecuteBindMaterial(m_BundleCommandList.get(), false);
            m_BindedMesh->ExecuteDraw(m_BundleCommandList.get(), m_BindedMaterial->GetShader()->GetBindSemanticFlag());

            m_BundleCommandList->Close();
        }
    }
}

