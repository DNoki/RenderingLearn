#pragma once


namespace Graphics
{
    typedef DirectX::VertexPositionNormalTexture VertexPositionNormalTexture;

    class IBufferResource;

    extern DescriptorHeap g_CommonSamplersDescriptorHeap;;

    extern DescriptorHandle g_SamplerPointBorder; // 点采样边界色纹理
    extern DescriptorHandle g_SamplerLinearBorder; // 线性采样边界色纹理
    extern DescriptorHandle g_SamplerPointClamp; // 点采样钳位纹理
    extern DescriptorHandle g_SamplerLinearClamp; // 线性采样钳位纹理
    extern DescriptorHandle g_SamplerPointWarp; // 点采样平铺纹理
    extern DescriptorHandle g_SamplerLinearWarp; // 线性采样平铺纹理
    extern DescriptorHandle g_SamplerPointMirror; // 点采样镜像纹理
    extern DescriptorHandle g_SamplerLinearMirror; // 线性采样镜像纹理


    class Mesh
    {
    public:
        Mesh() = default;

        inline UINT GetVertexBufferSize() const { return static_cast<UINT>(m_Vertices.size() * sizeof(VertexPositionNormalTexture)); }
        inline UINT GetVertexCount() const { return static_cast<UINT>(m_Vertices.size()); }
        inline const void* GetVertexData() const { return m_Vertices.data(); }

        inline UINT GetIndexBufferSize() const { return static_cast<UINT>(m_Indexes.size() * sizeof(UINT16)); }
        inline UINT GetIndexCount() const { return static_cast<UINT>(m_Indexes.size()); }
        inline const void* GetIndexData() const { return m_Indexes.data(); }

        inline const IBufferResource* GetVertexBuffer() const { return m_VertexBuffer.get(); }
        inline const IBufferResource* GetIndexBuffer() const { return m_IndexBuffer.get(); }

        static Mesh CreateCube(float size = 1, bool rhcoords = false);

    private:
        std::vector<VertexPositionNormalTexture> m_Vertices;
        std::vector<UINT16> m_Indexes;

        std::unique_ptr<IBufferResource> m_VertexBuffer;
        std::unique_ptr<IBufferResource> m_IndexBuffer;
    };
}