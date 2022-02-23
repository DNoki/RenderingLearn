#include "pch.h"

#include "GraphicsCore.h"

#include "PipelineState.h"

// --------------------------------------------------------------------------
/*
    管道状态概述 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/managing-graphics-pipeline-state-in-direct3d-12

    管道状态对象对应于图形处理单元 (GPU) 状态的重要部分。此状态包括所有当前设置的着色器和某些固定功能状态对象。
    更改管道对象中包含的状态的唯一方法是更改当前绑定的管道对象。

*/
// --------------------------------------------------------------------------

namespace Graphics
{
    GraphicsPipelineState::GraphicsPipelineState()
    {
        ZeroMemory(&m_PSODesc, sizeof(m_PSODesc));
        m_PSODesc.NodeMask = 1;
        m_PSODesc.SampleMask = UINT_MAX;
        m_PSODesc.SampleDesc.Count = 1;
        m_PSODesc.InputLayout = { nullptr, 0 };

        m_PSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        m_PSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        m_PSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        m_PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    }

    void GraphicsPipelineState::SetInputLayout(UINT numElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs)
    {
        m_PSODesc.InputLayout = { pInputElementDescs, numElements };
    }
    void GraphicsPipelineState::SetInputLayout(D3D12_INPUT_LAYOUT_DESC inputLayout)
    {
        m_PSODesc.InputLayout = inputLayout;
    }

    void GraphicsPipelineState::SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc)
    {
        m_PSODesc.RasterizerState = rasterizerDesc;
    }

    void GraphicsPipelineState::SetBlendState(const D3D12_BLEND_DESC& blendState)
    {
        m_PSODesc.BlendState = blendState;
    }

    void GraphicsPipelineState::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc)
    {
        m_PSODesc.DepthStencilState = depthStencilDesc;
    }

    void GraphicsPipelineState::SetSampleMask(UINT sampleMask)
    {
        m_PSODesc.SampleMask = sampleMask;
    }

    void GraphicsPipelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
    {
        ASSERT(topologyType != D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED, L"WARNING::Can't draw with undefined topology");
        m_PSODesc.PrimitiveTopologyType = topologyType;
    }

    void GraphicsPipelineState::SetIBStripCutValue(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE ibProps)
    {
        // 使用三角形带图元拓扑时，顶点位置被解释为连续三角形“带”的顶点。有一个特殊的索引值表示希望在条带中具有不连续性，即切割索引值。
        m_PSODesc.IBStripCutValue = ibProps;
    }

    /**
     * @brief 设置多渲染目标与深度模板格式
     * @param numRTVs 渲染目标数量
     * @param rtvFormats 渲染目标格式列表
     * @param dsvFormat 深度模板格式
     * @param msaaCount 多采样抗锯齿样本
     * @param msaaQuality 多采样抗锯齿质量
    */
    void GraphicsPipelineState::SetRenderTargetFormats(UINT numRTVs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat, UINT msaaCount, UINT msaaQuality)
    {
        ASSERT(numRTVs == 0 || rtvFormats != nullptr, L"WARNING::Null format array conflicts with non-zero length");

        m_PSODesc.NumRenderTargets = numRTVs; // 渲染目标视图数量（最大为8）
        CopyMemory(m_PSODesc.RTVFormats, rtvFormats, sizeof(DXGI_FORMAT) * numRTVs); // 渲染目标视图格式
        m_PSODesc.DSVFormat = dsvFormat; // 深度模板视图格式

        // 多采样抗锯齿样本与质量
        // 如果使用多样本抗锯齿，则所有绑定渲染目标和深度缓冲区必须具有相同的样本计数和质量级别。
        m_PSODesc.SampleDesc = { msaaCount, msaaQuality };

        // 使用以下方法查询多采样抗锯齿质量
        //D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msLevels;
        //msLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Replace with your render target format.
        //msLevels.SampleCount = 4; // Replace with your sample count.
        //msLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;

        //CHECK_HRESULT(Graphics::g_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msLevels, sizeof(msLevels)));
        //auto supportMsaaQuality = msLevels.NumQualityLevels;
    }

    void GraphicsPipelineState::SetVertexShader(const ID3DBlob* vs)
    {
        m_PSODesc.VS = CD3DX12_SHADER_BYTECODE(const_cast<ID3DBlob*>(vs));
    }

    void GraphicsPipelineState::SetPixelShader(const ID3DBlob* ps)
    {
        m_PSODesc.PS = CD3DX12_SHADER_BYTECODE(const_cast<ID3DBlob*>(ps));
    }

    void GraphicsPipelineState::SetGeometryShader(const ID3DBlob* gs)
    {
        m_PSODesc.GS = CD3DX12_SHADER_BYTECODE(const_cast<ID3DBlob*>(gs));
    }

    void GraphicsPipelineState::SetHullShader(const ID3DBlob* hs)
    {
        m_PSODesc.HS = CD3DX12_SHADER_BYTECODE(const_cast<ID3DBlob*>(hs));
    }

    void GraphicsPipelineState::SetDomainShader(const ID3DBlob* ds)
    {
        m_PSODesc.DS = CD3DX12_SHADER_BYTECODE(const_cast<ID3DBlob*>(ds));
    }

    /**
     * @brief 最终确认
    */
    void GraphicsPipelineState::Finalize()
    {
        /*
            运行时验证：
                ·着色器阶段之间的链接是否正确。
                ·如果指定了HS和DS成员，则拓扑类型的PrimitiveTopologyType成员必须设置为D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH。
                ·中心多采样抗锯齿 (MSAA) 模式是否允许采样频率执行。
                ·中心 MSAA 模式是否不允许使用抗锯齿线。
                ·如果RasterizerState指定的D3D12_RASTERIZER_DESC的ForcedSampleCount成员不为零：
                    ·必须禁用深度/模板。
                    ·像素着色器无法输出深度。
                    ·像素着色器无法以采样频率运行。
                    ·渲染目标样本数必须为 1。
                ·混合状态是否与渲染目标格式兼容。
                ·像素着色器输出类型是否与渲染目标格式兼容。
                ·渲染目标/深度模板格式是否支持样本计数和质量。
        */

        m_PSODesc.pRootSignature = m_RootSignature->GetD3D12RootSignature();
        ASSERT(m_PSODesc.pRootSignature != nullptr);
        //m_PSODesc.StreamOutput=
        //m_PSODesc.NodeMask=
        //m_PSODesc.CachedPSO=
        //m_PSODesc.Flags=

        m_PSO = nullptr;
        CHECK_HRESULT(Graphics::g_Device->CreateGraphicsPipelineState(&m_PSODesc, IID_PPV_ARGS(m_PSO.put())));
        SET_DEBUGNAME(m_PSO.get(), _T("PipelineState"));

    }
}
