#pragma once

#include <fstream>

#include "D3D12Core.h"

// --------------------------------------------------------------------------
// DirectXTK12 工具包 https://github.com/Microsoft/DirectXTK12
#include <SimpleMath.h> // 简单数学库封装
#include <Keyboard.h>
#include <Mouse.h>
#include <GeometricPrimitive.h> // 用于创建预置模型
#include <VertexTypes.h>

namespace D3D12Viewer
{
    using D3D12Core::uint8;
    using D3D12Core::uint16;
    using D3D12Core::uint32;
    using D3D12Core::uint64;
    using D3D12Core::int8;
    using D3D12Core::int16;
    using D3D12Core::int32;
    using D3D12Core::int64;
    using D3D12Core::ANSICHAR;
    using D3D12Core::WIDECHAR;
    using D3D12Core::UTF8CHAR;
    using D3D12Core::UTF16CHAR;
    using D3D12Core::UTF32CHAR;

    using D3D12Core::Path;
    using D3D12Core::String;
    using D3D12Core::UniquePtr;
    using D3D12Core::SharedPtr;
    using D3D12Core::WeakPtr;
    using D3D12Core::Vector;
    using D3D12Core::Queue;
    using D3D12Core::Set;
    using D3D12Core::Map;


    enum class VertexSemantic
    {
        Position,
        Normal,
        Tangent,
        Color,
        Texcoord,

        Count
    };

    class GraphicsManager;
    class RenderPass;
    class Mesh;
    class Material;
}

#include "Utility/MathCommon.h"
#include "Rendering/GraphicsManager.h"
#include "Rendering/RenderPass.h"

#include "Resource/Shader.h"
#include "Resource/Material.h"
#include "Resource/Mesh.h"
