﻿#pragma once

#include "RootSignature.h"

namespace Graphics
{
    /**
     * @brief 管线状态
    */
    class PipelineState
    {
    public:
        /**
         * @brief 获取D3D12管线状态对象
         * @return
        */
        inline ID3D12PipelineState* GetD3D12PSO() const { return m_PSO; }

    protected:
        ID3D12PipelineState* m_PSO;  // 管线状态对象

        PipelineState() : m_PSO(nullptr) {}

        virtual void Finalize() = 0;
    };

    /**
     * @brief 图形管线状态
    */
    class GraphicsPipelineState : public PipelineState
    {
    public:
        GraphicsPipelineState();

        inline D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetPsoDesc() { return m_PSODesc; }
        /**
         * @brief 检测是否已更改管线状态
         * @return
        */
        inline bool CheckStateChanged() const
        {
            return m_PsoDescHash != std::hash<D3D12_GRAPHICS_PIPELINE_STATE_DESC>::_Do_hash(m_PSODesc);
        }

        /**
         * @brief 设置根签名
         * @param rootSignature
        */
        inline void SetRootSignature(const RootSignature* rootSignature)
        {
            m_PSODesc.pRootSignature = rootSignature->GetD3D12RootSignature();
        }
        /**
         * @brief 设置管线使用的输入结构
         * @param numElements 输入元素数量
         * @param pInputElementDescs 输入元素表
        */
        void SetInputLayout(UINT numElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs);
        void SetInputLayout(const D3D12_INPUT_LAYOUT_DESC& inputLayout);
        /**
         * @brief 设置栅格化状态
         * @param rasterizerDesc
        */
        void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc);
        inline D3D12_RASTERIZER_DESC& GetRasterizerState() { return m_PSODesc.RasterizerState; }
        /**
         * @brief 设置混合状态
         * @param blendState
        */
        void SetBlendState(const D3D12_BLEND_DESC& blendState);
        inline D3D12_BLEND_DESC& GetBlendState() { return m_PSODesc.BlendState; }
        /**
         * @brief 设置深度模板状态
         * @param depthStencilDesc
        */
        void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);
        inline D3D12_DEPTH_STENCIL_DESC& GetDepthStencilState() { return m_PSODesc.DepthStencilState; }

        /**
         * @brief SampleMask（极少用到，用途未知）
         * @param sampleMask
        */
        void SetSampleMask(UINT sampleMask);
        /**
         * @brief 指定管道如何解释几何体或外壳着色器输入图元
         * @param topologyType
        */
        void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType);
        /**
         * @brief 指定索引缓冲区的属性（极少用到）
         * @param ibProps
        */
        void SetIBStripCutValue(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE ibProps);

        /**
         * @brief 设置多渲染目标格式
         * @param numRTVs 渲染目标数量
         * @param rtvFormats 渲染目标格式列表
         * @param dsvFormat 深度模板格式
         * @param msaaCount 多采样抗锯齿样本
         * @param msaaQuality 多采样抗锯齿质量
        */
        void SetRenderTargetFormats(UINT numRTVs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat, UINT msaaCount = 1, UINT msaaQuality = 0);

        /**
         * @brief 设置顶点着色器
         * @param vs
        */
        void SetVertexShader(const ID3DBlob* vs);
        /**
         * @brief 设置像素着色器
         * @param ps
        */
        void SetPixelShader(const ID3DBlob* ps);
        /**
         * @brief 设置几何着色器
         * @param gs
        */
        void SetGeometryShader(const ID3DBlob* gs);
        /**
         * @brief 设置外壳着色器
         * @param hs
        */
        void SetHullShader(const ID3DBlob* hs);
        /**
         * @brief 设置域着色器
         * @param ds
        */
        void SetDomainShader(const ID3DBlob* ds);

        /**
         * @brief 最终确认
        */
        virtual void Finalize();

    private:
        UINT64 m_PsoDescHash; // 已生成管线状态对象所使用的描述哈希值
        D3D12_GRAPHICS_PIPELINE_STATE_DESC m_PSODesc; // 图形管线状态描述

    };

    class ComputePipelineState : public PipelineState
    {
        // TODO
    };
}
