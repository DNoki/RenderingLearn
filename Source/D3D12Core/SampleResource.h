#pragma once

class RootSignature;
class GraphicsPipelineState;
class GpuBuffer;
class DescriptorHeap;
class Texture2D;
class GpuPlacedHeap;

namespace SampleResource
{
    extern RootSignature g_RootSignature;
    extern GraphicsPipelineState g_PipelineState;


    extern GpuBuffer g_SampleVBV;

    extern DescriptorHeap t_TexDH;
    extern Texture2D t_DefaultTexture;
    extern GpuPlacedHeap g_PlacedHeap;
    extern GpuPlacedHeap g_UploadPlacedHeap;


    void InitRootSignature();
    void InitPipelineState();
    void InitMesh();
    void InitPlacedHeap();
    void InitTexture2D();

    void SampleDraw(ID3D12GraphicsCommandList* commandList);
}