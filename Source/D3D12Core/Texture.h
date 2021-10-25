#pragma once

#include "GpuResource.h"

class Texture : public GpuResource
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


class Texture2D : public Texture
{
public:
    Texture2D() {}

    void GenerateChecker(const DescriptorHandle& pDescriptorHandle, UINT width, UINT height);

private:
    winrt::com_ptr<ID3D12Resource1> m_UploadResource;
};