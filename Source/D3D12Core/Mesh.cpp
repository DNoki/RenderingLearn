#include "pch.h"

#include "GraphicsCore.h"
#include "GraphicsBuffer.h"
#include "CommandList.h"

#include "Mesh.h"


// --------------------------------------------------------------------------
/*
    基元拓扑 https://docs.microsoft.com/zh-cn/windows/win32/direct3d11/d3d10-graphics-programming-guide-primitive-topologies
*/
// --------------------------------------------------------------------------


using namespace std;
using namespace DirectX;
using namespace Graphics;


namespace Game
{
#if 0
    void Mesh::DirectCreate(D3D_PRIMITIVE_TOPOLOGY primitiveTopology, UINT vertexCount, UINT strideSize, const void* vertices, UINT indexCount, const UINT16* indices)
    {
        m_VertexStrideSize = strideSize;
        m_PrimitiveTopology = primitiveTopology;

        auto vertexBuffer = unique_ptr<GraphicsBuffer>(new GraphicsBuffer());
        vertexBuffer->DirectCreate(vertexCount * m_VertexStrideSize);
        vertexBuffer->DispatchCopyBuffer(g_GraphicsCommandList, m_VertexStrideSize, vertices);
        m_VertexBuffer = move(vertexBuffer);

        m_Vertices.resize(m_VertexBuffer->GetBufferSize());
        CopyMemory(m_Vertices.data(), vertices, m_VertexBuffer->GetBufferSize());

        if (indexCount > 0)
        {
            auto indexBuffer = unique_ptr<GraphicsBuffer>(new GraphicsBuffer());
            indexBuffer->DirectCreate(indexCount * sizeof(UINT16));
            indexBuffer->DispatchCopyBuffer(g_GraphicsCommandList, sizeof(UINT16), indices);
            m_IndexBuffer = move(indexBuffer);

            m_Indices.resize(indexCount);
            CopyMemory(m_Indices.data(), indices, m_IndexBuffer->GetBufferSize());
        }
    }
#endif
    void Mesh::Finalize(D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
    {
        m_PrimitiveTopology = primitiveTopology;

        struct VertexInputLayout
        {
            Vector3 Position;
            Vector3 Normal;
            Vector3 Tangent;
            Vector4 Color;
            Vector2 UV;
        };

        UINT vertexCount = static_cast<UINT>(m_Vertices.size());
        UINT normalCount = static_cast<UINT>(m_Normals.size());
        UINT tangentCount = static_cast<UINT>(m_Tangents.size());
        UINT colorCount = static_cast<UINT>(m_Colors.size());
        UINT uvCount = static_cast<UINT>(m_UVs.size());

        if (normalCount != vertexCount)
        {
            normalCount = 0;
            m_Normals.resize(vertexCount);
        }
        if (tangentCount != vertexCount)
        {
            tangentCount = 0;
            m_Tangents.resize(vertexCount);
        }
        if (colorCount != vertexCount)
        {
            colorCount = 0;
            m_Colors.resize(vertexCount);
        }
        if (uvCount != vertexCount)
        {
            uvCount = 0;
            m_UVs.resize(vertexCount);
        }

        vector<VertexInputLayout> vertexArray;
        vertexArray.resize(vertexCount);
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertexArray[i].Position = m_Vertices[i];
            vertexArray[i].Normal = m_Normals[i];
            vertexArray[i].Tangent = m_Tangents[i];
            vertexArray[i].Color = m_Colors[i];
            vertexArray[i].UV = m_UVs[i];
        }

        m_VertexBuffer.reset(new GraphicsBuffer());
        m_VertexBuffer->DirectCreate(vertexCount * sizeof(VertexInputLayout));
        m_VertexBuffer->DispatchCopyBuffer(g_GraphicsCommandList, sizeof(VertexInputLayout), vertexArray.data());

        if (m_Indices.size() > 0)
        {
            m_IndexBuffer.reset(new GraphicsBuffer());
            m_IndexBuffer->DirectCreate(m_Indices.size() * sizeof(UINT16));
            m_IndexBuffer->DispatchCopyBuffer(g_GraphicsCommandList, sizeof(UINT16), m_Indices.data());
        }
    }

    void Mesh::ExecuteDraw(const Graphics::CommandList* commandList) const
    {
        auto* d3d12CommandList = commandList->GetD3D12CommandList();

        d3d12CommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

        d3d12CommandList->IASetVertexBuffers(0, 1, m_VertexBuffer->GetVBV());
        switch (GetDrawType())
        {
        case Game::DrawType::VertexList:
            d3d12CommandList->DrawInstanced(GetVertexCount(), 1, 0, 0);
            break;
        case Game::DrawType::Indexed:
            d3d12CommandList->IASetIndexBuffer(m_IndexBuffer->GetIBV());
            d3d12CommandList->DrawIndexedInstanced(GetIndexCount(), 1, 0, 0, 0);
            break;
        default: break;
        }
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
        mesh.m_Vertices.clear();
        mesh.m_Normals.clear();
        mesh.m_Tangents.clear();
        mesh.m_Colors.clear();
        mesh.m_UVs.clear();

        for (int i = 0; i < vertices.size(); i++)
        {
            mesh.m_Vertices.push_back(vertices[i].position);
            mesh.m_Normals.push_back(vertices[i].normal);
            mesh.m_UVs.push_back(vertices[i].textureCoordinate);
        }

        mesh.m_Indices.resize(indices.size());
        CopyMemory(mesh.m_Indices.data(), indices.data(), indices.size() * sizeof(UINT16));
        mesh.Finalize(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

}

