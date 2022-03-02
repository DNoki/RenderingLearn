#pragma once

namespace Graphics
{
    class RootSignature;
    class GraphicsPipelineState;
    class GraphicsBuffer;
    class DescriptorHeap;


    extern GraphicsBuffer g_SampleVBV;

    extern DescriptorHeap t_SampleResDescHeap;
#if 0
    extern PlacedHeap g_TexPlacedHeap;
    extern PlacedHeap g_UploadPlacedHeap;
#endif


    void InitShader();
    void InitMesh();
    void InitTexture2D();
    void InitCommandListBundle();

    void SampleDraw(const CommandList* g_GraphicsCommandList);
}