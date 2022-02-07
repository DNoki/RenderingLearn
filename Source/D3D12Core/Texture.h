﻿#pragma once

#include "IResource.h"

#if 0
#include "GraphicsResource.h"
// TODO 将贴图从 GraphicsResource 类解耦出来？

class Texture : public GraphicsResource
{
public:


protected:
    UINT m_Width;
    UINT m_Height;

    Texture() : m_Width(), m_Height() {}
};


/**
 * @brief 渲染贴图
*/
class RenderTexture : public Texture
{
public:
    RenderTexture();

    void Create(const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, const DescriptorHandle& pDescriptorHandle, UINT width, UINT height);
    void CreateFromSwapChain(UINT index, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, const DescriptorHandle& pDescriptorHandle);

protected:
    std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC> m_RtvDesc;

};
#endif

/**
 * @brief 渲染贴图
*/
class RenderTexture : public ITexture
{
public:
    RenderTexture();

    void DirectCreate(); // TODO
    void PlacedCreate(GpuPlacedHeap& pPlacedHeap); // TODO
    //void Create(const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, const DescriptorHandle& pDescriptorHandle, UINT width, UINT height);
    void CreateFromSwapChain(UINT index, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc);

protected:
    std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC> m_RtvDesc;

};