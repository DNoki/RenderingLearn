#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION //通过定义STB_IMAGE_IMPLEMENTATION，预处理器会修改头文件，让其只包含相关的函数定义源码，等于是将这个头文件变为一个 .cpp 文件了。
#define STBI_WINDOWS_UTF8 // STBI 文件路径字符串格式为 UTF8
#pragma warning(push, 0)
#pragma warning(disable:28251)
#include <stb_image/stb_image.h>
#pragma warning(pop)


using namespace D3D12Core;
using namespace D3D12Viewer;

namespace D3D12Viewer
{
    bool LoadTexture2D(const Path& inPath,
        Vector<uint8>& outData, DXGI_FORMAT& outFormat,
        int32& outWidth, int32& outHeight)
    {
        outData.clear();

        int32 nrChannels;

        stbi_set_flip_vertically_on_load(true);// 翻转纹理

        // 路径字符串转换
        UTF8CHAR string_path[MAX_PATH]{};
        WC2MB_Impl(CP_UTF8, inPath.c_str(), reinterpret_cast<ANSICHAR*>(string_path));

        // 加载并生成纹理
        const auto data = stbi_load(reinterpret_cast<ANSICHAR*>(string_path), &outWidth, &outHeight, &nrChannels, 0);
        const auto pixelCount = outWidth * outHeight;

        outFormat = DXGI_FORMAT_UNKNOWN;
        switch (nrChannels)
        {
        case 1:
            outFormat = DXGI_FORMAT_R8_UNORM;
            outData.resize(1ull * pixelCount);
            memcpy(outData.data(), data, outData.size());
            break;
        case 2:
            outFormat = DXGI_FORMAT_R8G8_UNORM;
            outData.resize(2ull * pixelCount);
            memcpy(outData.data(), data, outData.size());
            break;
        case 3:
        {
            outFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            outData.resize(4ull * pixelCount);
            size_t srcIndex = 0;
            for (size_t i = 0; i < outData.size(); i++)
            {
                if (i % 4 == 3)
                {
                    outData[i] = 0xFFu;
                    continue;
                }
                outData[i] = data[srcIndex++];
            }
        }
        break;
        case 4:
            outFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            outData.resize(4ull * pixelCount);
            memcpy(outData.data(), data, outData.size());
            break;
        default: ASSERT(0, TEXT("ERROR::未定义格式")); break;
        }

        // 释放资源
        stbi_image_free(data);

        return true;
    }
}
