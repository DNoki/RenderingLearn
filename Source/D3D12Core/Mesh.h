#pragma once


namespace Game
{
    typedef DirectX::VertexPositionNormalTexture VertexPositionNormalTexture;

    enum class DrawType
    {
        VertexList,
        Indexed,
    };

    constexpr UINT VertexSemanticCount = (int)VertexSemantic::Count;

    class Mesh
    {
    public:
        std::vector<Vector3> m_Positions;   // 位置
        std::vector<Vector3> m_Normals;     // 法线
        std::vector<Vector3> m_Tangents;    // 切线
        std::vector<Vector4> m_Colors;      // 颜色
        std::vector<Vector2> m_UVs;         // 纹理UV

        std::vector<UINT16> m_Indices;  // 索引列表

        // --------------------------------------------------------------------------
        Mesh() = default;

        // --------------------------------------------------------------------------
#if 0
        template <typename TVertex>
        void DirectCreate(D3D_PRIMITIVE_TOPOLOGY primitiveTopology, UINT vertexCount, const TVertex* vertices, UINT indexCount = 0, const UINT16* indices = nullptr)
        {
            DirectCreate(primitiveTopology, vertexCount, sizeof(TVertex), vertices, indexCount, indices);
        }
        void DirectCreate(D3D_PRIMITIVE_TOPOLOGY primitiveTopology, UINT vertexCount, UINT strideSize, const void* vertices, UINT indexCount = 0, const UINT16* indices = nullptr);
#endif
        void Finalize(D3D_PRIMITIVE_TOPOLOGY primitiveTopology);

        // --------------------------------------------------------------------------
        inline DrawType GetDrawType() const { return (m_IndexBuffer != nullptr ? DrawType::Indexed : DrawType::VertexList); }

        /**
         * @brief 执行检查资源屏障状态
         * @param commandList
        */
        void DispatchResourceExamine(const Graphics::CommandList* commandList) const;
        /**
         * @brief 执行绘制
         * @param commandList
         * @param bindSemanticFlag
        */
        void DispatchDraw(const Graphics::CommandList* commandList, int bindSemanticFlag) const;


        // --------------------------------------------------------------------------
        /**
         * @brief 创建面片
         * @param size
         * @param rhcoords
         * @return
        */
        static Mesh CreateQuad(float size = 1.0f, bool rhcoords = false);
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
        D3D_PRIMITIVE_TOPOLOGY m_PrimitiveTopology; // 定义管道如何解释和呈现顶点

        std::unique_ptr<Graphics::GraphicsBuffer> m_VertexBuffers[VertexSemanticCount]; // 顶点缓冲列表
        std::unique_ptr<Graphics::GraphicsBuffer> m_IndexBuffer;  // 索引缓冲

        std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW> m_VBVs[VertexSemanticCount]; // 顶点缓冲视图
        std::unique_ptr<D3D12_INDEX_BUFFER_VIEW> m_IBV; // 索引缓冲视图

        bool m_Version; // TODO

        /**
         * @brief 处理预置网格
        */
        static void ProcessPresetMesh(Mesh& mesh, DirectX::GeometricPrimitive::VertexCollection vertices, DirectX::GeometricPrimitive::IndexCollection indices);
    };
}