#pragma once

namespace Graphics
{
    class RootSignature;
    class GraphicsPipelineState;
    class GraphicsBuffer;
    class DescriptorHeap;
    class GpuPlacedHeap;

    extern RootSignature g_RootSignature;
    extern GraphicsPipelineState g_PipelineState;


    extern GraphicsBuffer g_SampleVBV;

    extern DescriptorHeap t_SampleResDescHeap;
    extern GpuPlacedHeap g_TexPlacedHeap;
    extern GpuPlacedHeap g_UploadPlacedHeap;


    void InitRootSignature();
    void InitPipelineState();
    void InitMesh();
    void InitPlacedHeap();
    void InitTexture2D();
    void InitCommandListBundle();

    void SampleDraw(ID3D12GraphicsCommandList* commandList);
}