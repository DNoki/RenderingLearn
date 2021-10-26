#pragma once

#include "Texture.h"
#include "UploadBuffer.h"

class GpuPlacedHeap;

class Texture2D : public Texture
{
public:
    Texture2D() {}

    void Create(const char* path, const DescriptorHandle& pDescriptorHandle);
    void Placed(const char* path, const DescriptorHandle& pDescriptorHandle, GpuPlacedHeap& pPlacedHeap, GpuPlacedHeap& pUploadPlacedHeap);

    void GenerateChecker(const DescriptorHandle& pDescriptorHandle, UINT width, UINT height);

private:
    std::unique_ptr<UploadBuffer> m_UploadBuffer;
};