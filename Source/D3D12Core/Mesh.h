#pragma once


namespace Graphics
{
    typedef DirectX::VertexPositionNormalTexture VertexPositionNormalTexture;

    class IBufferResource;

    class Mesh
    {
    public:
        // --------------------------------------------------------------------------
        Mesh() = default;

        // --------------------------------------------------------------------------
        template <typename TVertex>
        void DirectCreate(UINT vertexCount, const TVertex* vertices, UINT indexCount = 0, const UINT16* indices = nullptr)
        {
            DirectCreate(vertexCount, sizeof(TVertex), vertices, indexCount, indices);
        }
        void DirectCreate(UINT vertexCount, UINT strideSize, const void* vertices, UINT indexCount = 0, const UINT16* indices = nullptr);

        // --------------------------------------------------------------------------
        inline UINT GetVertexCount() const { return static_cast<UINT>(m_Vertices.size()); }
        inline const void* GetVertexData() const { return m_Vertices.data(); }

        inline UINT GetIndexCount() const { return static_cast<UINT>(m_Indices.size()); }
        inline const void* GetIndexData() const { return m_Indices.data(); }

        inline const GraphicsBuffer* GetVertexBuffer() const { return m_VertexBuffer.get(); }
        inline const GraphicsBuffer* GetIndexBuffer() const { return m_IndexBuffer.get(); }


        // --------------------------------------------------------------------------

        /**
         * @brief 创建立方体
         * @param size
         * @param rhcoords
         * @return
        */
        static Mesh CreateCube(float size = 1.0f, bool rhcoords = false);
        /**
         * @brief 创建球体（经纬型）
         * @param diameter 直径
         * @param tessellation
         * @param rhcoords
         * @param invertn
         * @return
        */
        static Mesh CreateSphere(float diameter = 1.0f, size_t tessellation = 16, bool rhcoords = false, bool invertn = false);
        /**
         * @brief 创建球体（三角型）
         * @param diameter
         * @param tessellation
         * @param rhcoords
         * @return
        */
        static Mesh CreateGeoSphere(float diameter = 1, size_t tessellation = 3, bool rhcoords = false);
        /**
         * @brief 创建圆柱体
         * @param height
         * @param diameter
         * @param tessellation
         * @param rhcoords
         * @return
        */
        static Mesh CreateCylinder(float height = 1, float diameter = 1, size_t tessellation = 32, bool rhcoords = false);
        /**
         * @brief 创建圆锥体
         * @param diameter 直径
         * @param height 高度
         * @param tessellation 细分
         * @param rhcoords
         * @return
        */
        static Mesh CreateCone(float diameter = 1, float height = 1, size_t tessellation = 32, bool rhcoords = false);
        /**
         * @brief 创建圆环体
         * @param diameter
         * @param thickness
         * @param tessellation
         * @param rhcoords
         * @return
        */
        static Mesh CreateTorus(float diameter = 1, float thickness = 0.333f, size_t tessellation = 32, bool rhcoords = false);
        /**
         * @brief 创建四面体（三角锥）
         * @param size
         * @param rhcoords
         * @return
        */
        static Mesh CreateTetrahedron(float size = 1, bool rhcoords = false);
        /**
         * @brief 创建八面体
         * @param size
         * @param rhcoords
         * @return
        */
        static Mesh CreateOctahedron(float size = 1, bool rhcoords = false);
        /**
         * @brief 创建十二面体
         * @param size
         * @param rhcoords
         * @return
        */
        static Mesh CreateDodecahedron(float size = 1, bool rhcoords = false);
        /**
         * @brief 创建二十面体
         * @param size
         * @param rhcoords
         * @return
        */
        static Mesh CreateIcosahedron(float size = 1, bool rhcoords = false);
        /**
         * @brief 创建犹他茶壶
         * @param size
         * @param tessellation
         * @param rhcoords
         * @return
        */
        static Mesh CreateTeapot(float size = 1, size_t tessellation = 8, bool rhcoords = false);

    private:
        UINT m_VertexStrideSize; // 顶点输入结构大小
        std::vector<BYTE> m_Vertices;
        std::vector<UINT16> m_Indices;

        std::unique_ptr<GraphicsBuffer> m_VertexBuffer;
        std::unique_ptr<GraphicsBuffer> m_IndexBuffer;
    };
}