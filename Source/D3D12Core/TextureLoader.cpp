#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION //通过定义STB_IMAGE_IMPLEMENTATION，预处理器会修改头文件，让其只包含相关的函数定义源码，等于是将这个头文件变为一个 .cpp 文件了。
#define STBI_WINDOWS_UTF8 // STBI 文件路径字符串格式为 UTF8
#include <stb_image.h>

//#include <wincodec.h> // TODO WIC库加载图片
//#include <DDSTextureLoader.h>
//#include <WICTextureLoader.h>

#include "TextureLoader.h"

TextureLoader::~TextureLoader()
{
}

void TextureLoader::LoadTexture2D(const Path& path)
{
    m_Data.clear();

    // 加载并生成纹理
    int nrChannels;

    stbi_set_flip_vertically_on_load(true);// 翻转纹理
    auto string_path = Utility::ToUtf8(path.c_str());
    UINT8* data = stbi_load(string_path.c_str(), &m_Width, &m_Height, &nrChannels, 0);
    int pixilCount = m_Width * m_Height;

    m_Format = DXGI_FORMAT_UNKNOWN;
    switch (nrChannels)
    {
    case 1:
        m_Format = DXGI_FORMAT_R8_UNORM;
        m_Data.resize(1 * pixilCount);
        memcpy(m_Data.data(), data, m_Data.size());
        break;
    case 2:
        m_Format = DXGI_FORMAT_R8G8_UNORM;
        m_Data.resize(2 * pixilCount);
        memcpy(m_Data.data(), data, m_Data.size());
        break;
    case 3:
    {
        m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        m_Data.resize(4 * pixilCount);
        size_t srcIndex = 0;
        for (size_t i = 0; i < m_Data.size(); i++)
        {
            if (i % 4 == 3)
            {
                m_Data[i] = 0xFFu;
                continue;
            }
            m_Data[i] = data[srcIndex++];
        }
    }
    break;
    case 4:
        m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        m_Data.resize(4 * pixilCount);
        memcpy(m_Data.data(), data, m_Data.size());
        break;
    default: ASSERT(0, L"ERROR::未定义格式"); break;
    }

    // 释放资源
    stbi_image_free(data);
}
