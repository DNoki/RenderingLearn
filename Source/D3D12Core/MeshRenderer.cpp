#include "pch.h"

#include "GraphicsCore.h"
#include "DescriptorHeap.h"
#include "GraphicsBuffer.h"
#include "CommandList.h"
#include "PipelineState.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

#include "MeshRenderer.h"

// --------------------------------------------------------------------------
/*
    光栅化渲染设计思路

    ·网格渲染器
            绑定一组网格和材质，由图形命令列表调用执行绘制
            用户可以选择使用捆绑包记录该渲染器的绘制命令，以加速绘制

        ·网格
                网格应是可以重复使用的资源。
                顶点输入元素分别存储在一组列表里，使用不同的槽位绑定。不同的语义设计为具有固定的槽位（有效值0~15）

        ·材质
                材质应是可以重复使用的资源。
                材质绑定一个着色器，以及管线状态对象、资源描述符堆、采样器描述符（通用）
                绑定材质时应考虑两点情况：
                    ①修改了管线状态对象的状态（需要重新生成管线状态对象）
                    ②重新绑定了使用的资源
                以上两种情况发生时，若使用捆绑包绘制，则需要重新记录捆绑包

            ·着色器
                    着色器应是可以重复使用的资源。
                    读取着色器汇编程序，记录根签名与顶点输入结构（使用语义）
                    记录使用资源类型与注册位置，记录使用的采样器
*/
// --------------------------------------------------------------------------


using namespace std;
using namespace Graphics;


namespace Game
{
    void MeshRenderer::Create(const Mesh* mesh, Material* material, bool useBundle)
    {
        m_BindedMesh = mesh;
        m_BindedMaterial = material;
        m_BindedMaterialVersion = 0;
        m_UseBundle = useBundle;

        if (m_UseBundle)
        {
            m_BundleCommandList.reset(new CommandList());
            m_BundleCommandList->Create(D3D12_COMMAND_LIST_TYPE_BUNDLE);
        }
        else m_BundleCommandList = nullptr;
    }

    void MeshRenderer::ExecuteDraw(const Graphics::CommandList* commandList)
    {
        // 必须由图形命令列表调用
        ASSERT(commandList->GetType() == D3D12_COMMAND_LIST_TYPE_DIRECT);

        // 设置当前渲染目标格式
        m_BindedMaterial->SetRenderTargets(commandList->GetRenderTargetInfos());

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
            // 重新捆绑命令列表
            if (m_UseBundle)
            {
                m_BundleCommandList->Reset();

                m_BindedMaterial->ExecuteBindMaterial(m_BundleCommandList.get(), false);
                m_BindedMesh->ExecuteDraw(m_BundleCommandList.get(), m_BindedMaterial->GetShader()->GetBindSemanticFlag());

                m_BundleCommandList->Close();
            }
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
}

