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

    void Mesh::ExecuteDraw(Graphics::CommandList* commandList)
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

        mesh.DirectCreate(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (UINT)vertices.size(), vertices.data(), (UINT)indices.size(), indices.data());
        return mesh;
    }

    Mesh Mesh::CreateSphere(float diameter, size_t tessellation, bool rhcoords, bool invertn)
    {
        using namespace DirectX;
        Mesh mesh;
        GeometricPrimitive::VertexCollection vertices;
        GeometricPrimitive::IndexCollection indices;
        GeometricPrimitive::CreateSphere(vertices, indices, diameter, tessellation, rhcoords, invertn);

        mesh.DirectCreate(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (UINT)vertices.size(), vertices.data(), (UINT)indices.size(), indices.data());
        return mesh;
    }

    Mesh Mesh::CreateGeoSphere(float diameter, size_t tessellation, bool rhcoords)
    {
        using namespace DirectX;
        Mesh mesh;
        GeometricPrimitive::VertexCollection vertices;
        GeometricPrimitive::IndexCollection indices;
        GeometricPrimitive::CreateGeoSphere(vertices, indices, diameter, tessellation, rhcoords);

        mesh.DirectCreate(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (UINT)vertices.size(), vertices.data(), (UINT)indices.size(), indices.data());
        return mesh;
    }

    Mesh Mesh::CreateCylinder(float height, float diameter, size_t tessellation, bool rhcoords)
    {
        using namespace DirectX;
        Mesh mesh;
        GeometricPrimitive::VertexCollection vertices;
        GeometricPrimitive::IndexCollection indices;
        GeometricPrimitive::CreateCylinder(vertices, indices, height, diameter, tessellation, rhcoords);

        mesh.DirectCreate(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (UINT)vertices.size(), vertices.data(), (UINT)indices.size(), indices.data());
        return mesh;
    }

    Mesh Mesh::CreateCone(float diameter, float height, size_t tessellation, bool rhcoords)
    {
        using namespace DirectX;
        Mesh mesh;
        GeometricPrimitive::VertexCollection vertices;
        GeometricPrimitive::IndexCollection indices;
        GeometricPrimitive::CreateCone(vertices, indices, diameter, height, tessellation, rhcoords);

        mesh.DirectCreate(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (UINT)vertices.size(), vertices.data(), (UINT)indices.size(), indices.data());
        return mesh;
    }

    Mesh Mesh::CreateTorus(float diameter, float thickness, size_t tessellation, bool rhcoords)
    {
        using namespace DirectX;
        Mesh mesh;
        GeometricPrimitive::VertexCollection vertices;
        GeometricPrimitive::IndexCollection indices;
        GeometricPrimitive::CreateTorus(vertices, indices, diameter, thickness, tessellation, rhcoords);

        mesh.DirectCreate(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (UINT)vertices.size(), vertices.data(), (UINT)indices.size(), indices.data());
        return mesh;
    }

    Mesh Mesh::CreateTetrahedron(float size, bool rhcoords)
    {
        using namespace DirectX;
        Mesh mesh;
        GeometricPrimitive::VertexCollection vertices;
        GeometricPrimitive::IndexCollection indices;
        GeometricPrimitive::CreateTetrahedron(vertices, indices, size, rhcoords);

        mesh.DirectCreate(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (UINT)vertices.size(), vertices.data(), (UINT)indices.size(), indices.data());
        return mesh;
    }

    Mesh Mesh::CreateOctahedron(float size, bool rhcoords)
    {
        using namespace DirectX;
        Mesh mesh;
        GeometricPrimitive::VertexCollection vertices;
        GeometricPrimitive::IndexCollection indices;
        GeometricPrimitive::CreateOctahedron(vertices, indices, size, rhcoords);

        mesh.DirectCreate(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (UINT)vertices.size(), vertices.data(), (UINT)indices.size(), indices.data());
        return mesh;
    }

    Mesh Mesh::CreateDodecahedron(float size, bool rhcoords)
    {
        using namespace DirectX;
        Mesh mesh;
        GeometricPrimitive::VertexCollection vertices;
        GeometricPrimitive::IndexCollection indices;
        GeometricPrimitive::CreateDodecahedron(vertices, indices, size, rhcoords);

        mesh.DirectCreate(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (UINT)vertices.size(), vertices.data(), (UINT)indices.size(), indices.data());
        return mesh;
    }

    Mesh Mesh::CreateIcosahedron(float size, bool rhcoords)
    {
        using namespace DirectX;
        Mesh mesh;
        GeometricPrimitive::VertexCollection vertices;
        GeometricPrimitive::IndexCollection indices;
        GeometricPrimitive::CreateIcosahedron(vertices, indices, size, rhcoords);

        mesh.DirectCreate(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (UINT)vertices.size(), vertices.data(), (UINT)indices.size(), indices.data());
        return mesh;
    }

    Mesh Mesh::CreateTeapot(float size, size_t tessellation, bool rhcoords)
    {
        using namespace DirectX;
        Mesh mesh;
        GeometricPrimitive::VertexCollection vertices;
        GeometricPrimitive::IndexCollection indices;
        GeometricPrimitive::CreateTeapot(vertices, indices, size, tessellation, rhcoords);

        mesh.DirectCreate(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (UINT)vertices.size(), vertices.data(), (UINT)indices.size(), indices.data());
        return mesh;
    }

}

