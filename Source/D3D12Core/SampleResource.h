#pragma once

namespace Graphics
{
    class RootSignature;
    class GraphicsPipelineState;
    class GraphicsBuffer;
    class DescriptorHeap;
    class GpuPlacedHeap;

    //extern RootSignature g_RootSignature;
    //extern GraphicsPipelineState g_PipelineState;


    extern GraphicsBuffer g_SampleVBV;

    extern DescriptorHeap t_SampleResDescHeap;
#if 0
    extern GpuPlacedHeap g_TexPlacedHeap;
    extern GpuPlacedHeap g_UploadPlacedHeap;
#endif


    //void InitRootSignature();
    //void InitPipelineState();
    void InitShader();
    void InitMesh();
    void InitPlacedHeap();
    void InitTexture2D();
    void InitCommandListBundle();

    void SampleDraw();
}