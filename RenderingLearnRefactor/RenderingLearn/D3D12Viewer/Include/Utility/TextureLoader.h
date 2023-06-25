#pragma once

namespace D3D12Viewer
{
    /**
     * \brief 加载纹理资源
     * \param inPath 资源路径
     * \param outData
     * \param outFormat
     * \param outWidth
     * \param outHeight
     * \return
     */
    extern bool LoadTexture2D(const Path& inPath,
        Vector<uint8>& outData, DXGI_FORMAT& outFormat,
        int32& outWidth, int32& outHeight);
}