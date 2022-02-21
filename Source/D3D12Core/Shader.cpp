#include "pch.h"

#include <d3dcompiler.h>

#include "AppMain.h"
#include "PipelineState.h"
#include "CommandList.h"

#include "Shader.h"



using namespace std;
using namespace Graphics;


namespace Game
{

    void Shader::Create()
    {
        Path vsShaderPath = Application::GetShaderPath().append("SampleTexture_vs.cso");
        Path psShaderPath = Application::GetShaderPath().append("SampleTexture_ps.cso");

        ReadFromFile(ShaderType::VertexShader, vsShaderPath);
        ReadFromFile(ShaderType::PixelShader, psShaderPath);

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
}

