#include "pch.h"
#include "Resource/Mesh.h"


// --------------------------------------------------------------------------
/*
    输入汇编器 https://docs.microsoft.com/zh-cn/windows/win32/direct3d11/d3d10-graphics-programming-guide-input-assembler-stage-getting-started

    基元拓扑 https://docs.microsoft.com/zh-cn/windows/win32/direct3d11/d3d10-graphics-programming-guide-primitive-topologies
*/
// --------------------------------------------------------------------------

using namespace D3D12Core;
using namespace D3D12Viewer;

#if 0
void Mesh::DirectCreate(D3D_PRIMITIVE_TOPOLOGY primitiveTopology, uint32 vertexCount, uint32 strideSize, const void* vertices, uint32 indexCount, const uint16* indices)
{
    m_VertexStrideSize = strideSize;
    m_PrimitiveTopology = primitiveTopology;

    auto vertexBuffer = unique_ptr<GraphicsBuffer>(new GraphicsBuffer());
    vertexBuffer->DirectCreate(vertexCount * m_VertexStrideSize);
    vertexBuffer->DispatchUploadBuffer(g_GraphicsCommandList, m_VertexStrideSize, vertices);
    m_VertexBuffer = move(vertexBuffer);

    m_Vertices.resize(m_VertexBuffer->GetBufferSize());
    CopyMemory(m_Vertices.data(), vertices, m_VertexBuffer->GetBufferSize());

    if (indexCount > 0)
    {
        auto indexBuffer = unique_ptr<GraphicsBuffer>(new GraphicsBuffer());
        indexBuffer->DirectCreate(indexCount * sizeof(uint16));
        indexBuffer->DispatchUploadBuffer(g_GraphicsCommandList, sizeof(uint16), indices);
        m_IndexBuffer = move(indexBuffer);

        m_Indices.resize(indexCount);
        CopyMemory(m_Indices.data(), indices, m_IndexBuffer->GetBufferSize());
    }
}
#endif
void Mesh::Finalize(D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
{
    m_PrimitiveTopology = primitiveTopology;

    // 取得一个闲置拷贝命令列表
    auto* commandList = CommandListPool::Request<CopyCommandList>();
    commandList->Reset();

    // 顶点数量
    uint64 vertexCount = m_Positions.size();
    uint64 vertexCountArray[] = { m_Positions.size(), m_Normals.size(), m_Tangents.size(), m_Colors.size(), m_UVs.size(), };
    // 顶点语义数据类型
    uint32 vertexStrideArray[] = { sizeof(Vector3), sizeof(Vector3), sizeof(Vector3), sizeof(Vector4), sizeof(Vector2), };
    // 顶点语义数据
    void* vertexDataArray[] = { m_Positions.data(), m_Normals.data(), m_Tangents.data(), m_Colors.data(), m_UVs.data(), };

    // 创建需要使用的顶点缓冲
    for (size_t i = 0; i < VertexSemanticCount; i++)
    {
        m_VertexBuffers[i] = nullptr;
        m_VBVs[i] = nullptr;
        if (vertexCountArray[i] != vertexCount)
            continue;

        m_VertexBuffers[i].reset(new GraphicsBuffer());
        m_VertexBuffers[i]->PlacedCreate(vertexStrideArray[i] * vertexCountArray[i]);
        //m_VertexBuffers[i]->DispatchUploadBuffer(*commandList, vertexDataArray[i]);
        commandList->DispatchUploadBuffer(m_VertexBuffers[i].get(), vertexDataArray[i]);

        m_VBVs[i].reset(new D3D12_VERTEX_BUFFER_VIEW
            {
                m_VertexBuffers[i]->GetGpuVirtualAddress(),
                static_cast<uint32>(m_VertexBuffers[i]->GetBufferSize()),
                vertexStrideArray[i],
            });
    }

    // 如果使用索引绘制，则创建索引缓冲
    if (m_Indices.size() > 0)
    {
        m_IndexBuffer.reset(new GraphicsBuffer());
        m_IndexBuffer->PlacedCreate(m_Indices.size() * sizeof(uint16));
        //m_IndexBuffer->DispatchUploadBuffer(*commandList, m_Indices.data());
        commandList->DispatchUploadBuffer(m_IndexBuffer.get(), m_Indices.data());

        m_IBV.reset(new D3D12_INDEX_BUFFER_VIEW
            {
                m_IndexBuffer->GetGpuVirtualAddress(),
                static_cast<uint32>(m_IndexBuffer->GetBufferSize()),
                DXGI_FORMAT_R16_UINT, // uint16
            });
    }
    else m_IndexBuffer = nullptr;

    ICommandList* c = commandList;
    GraphicsManager::GetInstance().GetCopyCommandQueue()->ExecuteCommandLists(&c);
}

void Mesh::SetName(const String& name)
{
    m_Name = String(name);

    static const String vertexNames[] =
    {
        L"VBV位置",
        L"VBV法线",
        L"VBV切线",
        L"VBV颜色",
        L"VBV纹理坐标",
    };
    for (size_t i = 0; i < VertexSemanticCount; i++)
    {
        if (m_VertexBuffers[i])
        {
            GraphicsContext::SetDebugName(m_VertexBuffers[i]->GetD3D12Resource(), FORMAT(TEXT("%s %s"), m_Name.c_str(), vertexNames[i]));
        }
    }
    if (m_IndexBuffer)
    {
        GraphicsContext::SetDebugName(m_IndexBuffer->GetD3D12Resource(), FORMAT(TEXT("%s IBV索引"), m_Name.c_str()));
    }
}

void Mesh::DispatchResourceExamine(GraphicsCommandList* commandList) const
{
    for (size_t i = 0; i < VertexSemanticCount; i++)
    {
        if (m_VertexBuffers[i] && (m_VertexBuffers[i]->GetResourceStates() != D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER))
        {
            m_VertexBuffers[i]->DispatchTransitionStates(commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        }
    }
    if (m_IndexBuffer && (m_IndexBuffer->GetResourceStates() != D3D12_RESOURCE_STATE_INDEX_BUFFER))
        m_IndexBuffer->DispatchTransitionStates(commandList, D3D12_RESOURCE_STATE_INDEX_BUFFER);
}

void Mesh::DispatchDraw(GraphicsCommandList* commandList, int32 bindSemanticFlag) const
{
    commandList->IASetPrimitiveTopology(m_PrimitiveTopology);

    // 绑定顶点缓冲
    for (size_t i = 0; i < VertexSemanticCount; i++)
    {
        if (bindSemanticFlag & (1 << i))
        {
            ASSERT(m_VertexBuffers[i], _T("ERROR::未设置要使用的顶点缓冲。"));
            if (m_VertexBuffers[i])
                commandList->IASetVertexBuffers(static_cast<uint32>(i), m_VBVs[i].get());
        }
    }

    switch (GetDrawType())
    {
    case DrawType::VertexList:
        commandList->DrawInstanced(static_cast<uint32>(m_Positions.size()), 1, 0, 0);
        break;
    case DrawType::Indexed:
        commandList->IASetIndexBuffer(m_IBV.get()); // 绑定索引缓冲
        commandList->DrawIndexedInstanced(static_cast<uint32>(m_Indices.size()), 1, 0, 0, 0);
        break;
    default: break;
    }
}

void Mesh::DispatchDraw(GraphicsCommandList* commandList, Material* mat) const
{
    //// 必须由图形命令列表调用
    //ASSERT(commandList->GetType() == D3D12_COMMAND_LIST_TYPE_DIRECT);

    //// 设置当前渲染目标格式
    //mat->SetRenderTargets(commandList->GetRenderTargetInfos());

    //// 检测管线状态是否已更改
    //if (mat->GetPipelineState()->CheckStateChanged())
    //{
    //    // 重新创建管线状态
    //    mat->GetPipelineState()->Finalize();
    //}

    //mat->DispatchBindMaterial(commandList, false);
    //DispatchResourceExamine(commandList);
    //DispatchDraw(commandList, mat->GetShader()->GetBindSemanticFlag());
}

Mesh Mesh::CreateQuad(float size, bool rhcoords)
{
    Mesh mesh{};
    mesh.m_Positions =
    {
        Vector3(-0.5f, -0.5f, 0.0f), Vector3(-0.5f, 0.5f, 0.0f),
        Vector3(0.5f, -0.5f, 0.0f), Vector3(0.5f, 0.5f, 0.0f),
    };
    for (auto& pos : mesh.m_Positions)
        pos *= size;
    mesh.m_Normals =
    {
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
    };
    mesh.m_UVs =
    {
        Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f),
        Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),
    };

    if (rhcoords) mesh.m_Indices = { 0, 2, 1, 3 };
    else mesh.m_Indices = { 0, 1, 2, 3 };

    mesh.Finalize(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    return mesh;
}

Mesh Mesh::CreateCube(float size, bool rhcoords)
{
    using namespace DirectX;
    Mesh mesh;
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    GeometricPrimitive::CreateCube(vertices, indices, size, rhcoords);

    ProcessPresetMesh(mesh, vertices, indices);
    return mesh;
}
Mesh Mesh::CreateSphere(float diameter, size_t tessellation, bool rhcoords, bool invertn)
{
    using namespace DirectX;
    Mesh mesh;
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    GeometricPrimitive::CreateSphere(vertices, indices, diameter, tessellation, rhcoords, invertn);

    ProcessPresetMesh(mesh, vertices, indices);
    return mesh;
}
Mesh Mesh::CreateGeoSphere(float diameter, size_t tessellation, bool rhcoords)
{
    using namespace DirectX;
    Mesh mesh;
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    GeometricPrimitive::CreateGeoSphere(vertices, indices, diameter, tessellation, rhcoords);

    ProcessPresetMesh(mesh, vertices, indices);
    return mesh;
}
Mesh Mesh::CreateCylinder(float height, float diameter, size_t tessellation, bool rhcoords)
{
    using namespace DirectX;
    Mesh mesh;
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    GeometricPrimitive::CreateCylinder(vertices, indices, height, diameter, tessellation, rhcoords);

    ProcessPresetMesh(mesh, vertices, indices);
    return mesh;
}
Mesh Mesh::CreateCone(float diameter, float height, size_t tessellation, bool rhcoords)
{
    using namespace DirectX;
    Mesh mesh;
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    GeometricPrimitive::CreateCone(vertices, indices, diameter, height, tessellation, rhcoords);

    ProcessPresetMesh(mesh, vertices, indices);
    return mesh;
}
Mesh Mesh::CreateTorus(float diameter, float thickness, size_t tessellation, bool rhcoords)
{
    using namespace DirectX;
    Mesh mesh;
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    GeometricPrimitive::CreateTorus(vertices, indices, diameter, thickness, tessellation, rhcoords);

    ProcessPresetMesh(mesh, vertices, indices);
    return mesh;
}
Mesh Mesh::CreateTetrahedron(float size, bool rhcoords)
{
    using namespace DirectX;
    Mesh mesh;
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    GeometricPrimitive::CreateTetrahedron(vertices, indices, size, rhcoords);

    ProcessPresetMesh(mesh, vertices, indices);
    return mesh;
}
Mesh Mesh::CreateOctahedron(float size, bool rhcoords)
{
    using namespace DirectX;
    Mesh mesh;
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    GeometricPrimitive::CreateOctahedron(vertices, indices, size, rhcoords);

    ProcessPresetMesh(mesh, vertices, indices);
    return mesh;
}
Mesh Mesh::CreateDodecahedron(float size, bool rhcoords)
{
    using namespace DirectX;
    Mesh mesh;
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    GeometricPrimitive::CreateDodecahedron(vertices, indices, size, rhcoords);

    ProcessPresetMesh(mesh, vertices, indices);
    return mesh;
}
Mesh Mesh::CreateIcosahedron(float size, bool rhcoords)
{
    using namespace DirectX;
    Mesh mesh;
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    GeometricPrimitive::CreateIcosahedron(vertices, indices, size, rhcoords);

    ProcessPresetMesh(mesh, vertices, indices);
    return mesh;
}
Mesh Mesh::CreateTeapot(float size, size_t tessellation, bool rhcoords)
{
    using namespace DirectX;
    Mesh mesh;
    GeometricPrimitive::VertexCollection vertices;
    GeometricPrimitive::IndexCollection indices;
    GeometricPrimitive::CreateTeapot(vertices, indices, size, tessellation, rhcoords);

    ProcessPresetMesh(mesh, vertices, indices);
    return mesh;
}

void Mesh::ProcessPresetMesh(Mesh& mesh, DirectX::GeometricPrimitive::VertexCollection vertices, DirectX::GeometricPrimitive::IndexCollection indices)
{
    mesh.m_Positions.clear();
    mesh.m_Normals.clear();
    mesh.m_Tangents.clear();
    mesh.m_Colors.clear();
    mesh.m_UVs.clear();

    for (size_t i = 0; i < vertices.size(); i++)
    {
        mesh.m_Positions.push_back(vertices[i].position);
        mesh.m_Normals.push_back(vertices[i].normal);
        mesh.m_UVs.push_back(Vector2(vertices[i].textureCoordinate.x, 1.0f - vertices[i].textureCoordinate.y)); // 翻转 UV 纵坐标
    }

    mesh.m_Indices.resize(indices.size());
    CopyMemory(mesh.m_Indices.data(), indices.data(), indices.size() * sizeof(uint16));
    mesh.Finalize(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
