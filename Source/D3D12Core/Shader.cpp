#include "pch.h"

#include <d3dcompiler.h>

#include "AppMain.h"
#include "PipelineState.h"
#include "CommandList.h"

#include "Shader.h"

// --------------------------------------------------------------------------
/*
    HLSL 编译器 https://docs.microsoft.com/zh-cn/cpp/build/reference/hlsl-property-pages

*/
// --------------------------------------------------------------------------


using namespace std;
using namespace Graphics;


namespace Game
{

    void Shader::Create(const ShaderDesc* shaderDesc)
    {
#if 1
        if (shaderDesc == nullptr) throw L"ERROR::ShaderDesc is nullptr";
        m_ShaderDesc = *shaderDesc;

        // 加载着色器缓冲
        for (int i = 0; i < _countof(m_ShaderDesc.m_ShaderFilePaths); i++)
        {
            if (!m_ShaderDesc.m_ShaderFilePaths[i].empty())
                ReadFromFile(static_cast<ShaderType>(i), m_ShaderDesc.m_ShaderFilePaths[i]);
        }

        // 生成根签名
        {
            vector<CD3DX12_DESCRIPTOR_RANGE1> resourceDescriptorRanges{};
#if 0
            if (m_ShaderDesc.m_CbvCount > 0)
            {
                resourceDescriptorRanges.push_back(CD3DX12_DESCRIPTOR_RANGE1());
                resourceDescriptorRanges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, m_ShaderDesc.m_CbvCount, 0);
            }
#endif
            if (m_ShaderDesc.m_SrvCount > 0)
            {
                resourceDescriptorRanges.push_back(CD3DX12_DESCRIPTOR_RANGE1());
                resourceDescriptorRanges.back().Init(
                    D3D12_DESCRIPTOR_RANGE_TYPE_SRV, // 描述范围类型
                    m_ShaderDesc.m_SrvCount, // 描述符数量
                    0, // 起始注册位置
                    0, // 寄存器空间，通常可以为 0
                    D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE); // 指定描述符和数据易失性
            }
            if (m_ShaderDesc.m_UavCount > 0)
            {
                ASSERT(0); // TODO
            }

            vector<CD3DX12_DESCRIPTOR_RANGE1> sampleDescriptorRanges{};
            if (m_ShaderDesc.m_SamplerCount > 0)
            {
                sampleDescriptorRanges.push_back(CD3DX12_DESCRIPTOR_RANGE1());
                sampleDescriptorRanges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, m_ShaderDesc.m_SamplerCount, 0);
            }

            // 计算根参数数量
            UINT rootParamsCount = (resourceDescriptorRanges.size() > 0 ? 1 : 0) + (sampleDescriptorRanges.size() > 0 ? 1 : 0) + m_ShaderDesc.m_CbvCount;

            m_RootSignature.reset(new RootSignature());
            m_RootSignature->Reset(rootParamsCount, 0); // 不使用静态采样器

            UINT rootParamIndex = 0;

            // 根描述符
            for (int i = 0; i < m_ShaderDesc.m_CbvCount; i++)
            {
                (*m_RootSignature)[rootParamIndex++].InitAsConstantBufferView(i);
            }

            // CBV、SRV、UAV 描述符表
            if (resourceDescriptorRanges.size() > 0)
            {
                (*m_RootSignature)[rootParamIndex++].InitAsDescriptorTable(static_cast<UINT>(resourceDescriptorRanges.size()), resourceDescriptorRanges.data());
            }
            // 采样器描述符表
            if (sampleDescriptorRanges.size() > 0)
            {
                (*m_RootSignature)[rootParamIndex++].InitAsDescriptorTable(static_cast<UINT>(sampleDescriptorRanges.size()), sampleDescriptorRanges.data());
            }

            m_RootSignature->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        }
#endif
#if 0
        Path vsShaderPath = Application::GetShaderPath().append("SampleTexture_vs.cso");
        Path psShaderPath = Application::GetShaderPath().append("SampleTexture_ps.cso");

        ReadFromFile(ShaderType::VertexShader, vsShaderPath);
        ReadFromFile(ShaderType::PixelShader, psShaderPath);

        m_BindResourceCount = 2;

        {
            m_RootSignature.reset(new RootSignature());

            m_RootSignature->Reset(3, 0);

            CD3DX12_DESCRIPTOR_RANGE1 ranges[3]{};
            {
                ranges[0].Init(
                    D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                    1, 0, 0,
                    D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
                ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
                ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

                for (int i = 0; i < _countof(ranges); i++)
                    (*m_RootSignature)[i].InitAsDescriptorTable(1, &ranges[i]);
            }
            m_RootSignature->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        }

        m_BindSemanticFlag =
            (1 << (int)VertexSemantic::Position) |
            (1 << (int)VertexSemantic::Normal) |
            (1 << (int)VertexSemantic::Texcoord);
#endif

        // 定义顶点输入层
        {
            static const D3D12_INPUT_ELEMENT_DESC sampleInputElementDescs[] =
            {
                {"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, (int)VertexSemantic::Position, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}, // XMFLOAT3
                {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, (int)VertexSemantic::Normal, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}, // XMFLOAT3
                {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, (int)VertexSemantic::Tangent, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}, // XMFLOAT3
                {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, (int)VertexSemantic::Color, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}, // XMFLOAT4
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, (int)VertexSemantic::Texcoord, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}, // XMFLOAT2
            };
            m_InputLayouts.resize(_countof(sampleInputElementDescs));
            CopyMemory(m_InputLayouts.data(), sampleInputElementDescs, sizeof(sampleInputElementDescs));
        }
    }

    void Shader::ReadFromFile(ShaderType type, const Path& filePath)
    {
        auto& blob = m_ShaderBlobs[static_cast<int>(type)];
        ASSERT(blob == nullptr, _T("ERROR::已加载指定着色器类型。"));

        // C++ 文件读写操作
        //#include <ifstream> // 从文件读取
        //#include <ofstream> // 从文件读取
        //#include <fstream> // 打开文件供读写

        // 文件流打开模式
        // ios::in          只读
        // ios::out         只写
        // ios::app         从文件末尾开始写入
        // ios::binary      二进制模式
        // ios::nocreate    打开文件时若文件不存在，不创建文件
        // ios::noreplace   打开文件时若文件不存在，则创建文件
        // ios::trunc       打开文件并清空内容
        // ios::ate         打开文件并移动到文件尾

        // 状态标志服
        // is_open() 文件是否正常打开
        // bad() 读写过程中发生错误
        // fail() 读写过程中发生错误 或格式错误
        // eof() 读文件到达末尾
        // good() 以上任何一个返回true，则返回false

        // 文件流指针位置
        // ios::beg         文件头
        // ios::end         文件尾
        // ios::cur         当前位置

        // 文件流指针
        // tellg() 返回输入流指针的位置(类型为long)
        // tellp() 返回输出流指针的位置(类型为long)
        // seekg() 设置输入流指针位置，可偏移指定量
        // seekp() 设置输出流指针位置，可偏移指定量

        ifstream file;
        file.open(filePath, ios::binary);
        ASSERT(file.good(), L"ERROR::Shader文件打开失败。");

        file.seekg(0, ios_base::end);
        auto size = file.tellg();
        file.seekg(0, ios_base::beg);

        auto hresult = D3DCreateBlob(size, blob.put());
        CHECK_HRESULT(hresult);

        file.read(reinterpret_cast<char*>(blob.get()->GetBufferPointer()), size);
        file.close();
    }

#if 0
#if DEBUG
    HRESULT ShaderUtility::CompileFromFile(ShaderType type, Path& pFileName, ID3DBlob** ppCode)
    {
        static const string ShaderModel = "5_1";
        static const string entryPoints[] =
        {
            "VSMain",
            "PSMain",
            "GSMain",
            "HSMain",
            "DSMain",
        };
        static const string compileTarget[] =
        {
            Application::Format("vs_%s", ShaderModel.c_str()),
            Application::Format("ps_%s", ShaderModel.c_str()),
            Application::Format("gs_%s", ShaderModel.c_str()),
            Application::Format("hs_%s", ShaderModel.c_str()),
            Application::Format("ds_%s", ShaderModel.c_str()),
        };

        // CompileFlags https://docs.microsoft.com/zh-cn/windows/win32/direct3dhlsl/d3dcompile-constants
#ifdef DEBUG
    // 使用图形调试工具启用着色器调试
        auto compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        auto compileFlags = 0;
#endif // DEBUG

        HRESULT compileResult;
        winrt::com_ptr<ID3DBlob> errorBlob;

        // #define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)
        // D3D_COMPILE_STANDARD_FILE_INCLUDE 可以在任何 API 中为 pInclude 传递，并指示应该使用简单的默认包含处理程序。 包含处理程序将包含与当前目录相关的文件和与初始源文件目录相关的文件。 当与 D3DCompile 之类的 API 一起使用时，pSourceName 必须是文件名，并且初始相对目录将从它派生。

        compileResult = D3DCompileFromFile(
            pFileName.c_str(),                              // HLSL文件路径
            nullptr,                                // 定义着色器宏
            D3D_COMPILE_STANDARD_FILE_INCLUDE,      // 默认包含处理
            entryPoints[static_cast<int>(type)].c_str(),    // 着色器入口函数
            compileTarget[static_cast<int>(type)].c_str(),  // 着色器目标
            compileFlags,                           // 着色器编译选项
            0,                                      // 效果编译选项
            ppCode,                                 // 接收已编译的代码
            errorBlob.put()                         // 接收编译错误信息
        );
        if (FAILED(compileResult) && errorBlob)
            TRACE((char*)errorBlob->GetBufferPointer());

        CHECK_HRESULT(compileResult);

        return compileResult;
    }
#endif

    HRESULT ShaderUtility::ReadFromFile(ShaderType type, Path& pFileName, ID3DBlob** ppCode)
    {
        // C++ 文件读写操作
        //#include <ifstream> // 从文件读取
        //#include <ofstream> // 从文件读取
        //#include <fstream> // 打开文件供读写

        // 文件流打开模式
        // ios::in          只读
        // ios::out         只写
        // ios::app         从文件末尾开始写入
        // ios::binary      二进制模式
        // ios::nocreate    打开文件时若文件不存在，不创建文件
        // ios::noreplace   打开文件时若文件不存在，则创建文件
        // ios::trunc       打开文件并清空内容
        // ios::ate         打开文件并移动到文件尾

        // 状态标志服
        // is_open() 文件是否正常打开
        // bad() 读写过程中发生错误
        // fail() 读写过程中发生错误 或格式错误
        // eof() 读文件到达末尾
        // good() 以上任何一个返回true，则返回false

        // 文件流指针位置
        // ios::beg         文件头
        // ios::end         文件尾
        // ios::cur         当前位置

        // 文件流指针
        // tellg() 返回输入流指针的位置(类型为long)
        // tellp() 返回输出流指针的位置(类型为long)
        // seekg() 设置输入流指针位置，可偏移指定量
        // seekp() 设置输出流指针位置，可偏移指定量

        ifstream file;
        file.open(pFileName, ios::binary);
        ASSERT(file.good(), L"ERROR::Shader文件打开失败。");

        file.seekg(0, ios_base::end);
        auto size = file.tellg();
        file.seekg(0, ios_base::beg);

        auto hresult = D3DCreateBlob(size, ppCode);
        CHECK_HRESULT(hresult);

        file.read(reinterpret_cast<char*>((*ppCode)->GetBufferPointer()), size);
        file.close();

        return hresult;
    }
#endif
}

