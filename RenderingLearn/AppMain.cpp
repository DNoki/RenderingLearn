#include "pch.h"
#include "AppMain.h"




// --------------------------------------------------------------------------
// Direct3D https://docs.microsoft.com/zh-cn/windows/win32/direct3d
// Direct3D 12 图形 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/direct3d-12-graphics
#include <d3d12.h>
#include <d3dcompiler.h>
//#include <d3d12shader.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#include <d3dx12.h> // Direct3D 12 的帮助程序结构 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/helper-structures-for-d3d12

#include <DirectXMath.h> // 数学库


// --------------------------------------------------------------------------
// DXGI https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/dx-graphics-dxgi
#include <dxgi1_6.h>


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")





// 在调用COM接口时简化出错时处理时使用
//#define GRS_THROW_IF_FAILED(hr) if (FAILED(hr)){ throw CGRSCOMException(hr); }
//class CGRSCOMException
//{
//public:
//    CGRSCOMException(HRESULT hr) :_hrError(hr) {}
//    HRESULT Error() const { return _hrError; }
//
//private:
//    const HRESULT _hrError;
//};

//using namespace std;

//using namespace Microsoft;
//using namespace Microsoft::WRL;

using namespace DirectX;

constexpr UINT SWAP_FRAME_BACK_BUFFER_COUNT = 3; // 交换后缓冲数量

HWND g_hwnd;
bool ExitFlag;
int ScreenWidth = 1280;
int ScreenHeight = 720;
winrt::com_ptr<ID3D12Device6> g_Device;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATE: // 窗口正在激活或停用
        break;

    case WM_SIZE: // 在窗口的大小更改后发送到窗口

        break;

    case WM_CLOSE: // 窗口将要关闭时
        DestroyWindow(g_hwnd);
        break;

    case WM_DESTROY: // 窗口已关闭
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

int InitializeApplication(HINSTANCE hInstance, int nCmdShow)
{
    // 第一个 Windows 程序
    // https://docs.microsoft.com/zh-cn/windows/win32/learnwin32/creating-a-window
    auto title = L"TestClass";
    ExitFlag = false;
    auto screenWidth = ScreenWidth;
    auto screenHeight = ScreenHeight;

    // --------------------------------------------------------------------------
    // 注册窗口类 
    // https://docs.microsoft.com/zh-cn/windows/win32/winmsg/using-window-classes
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_GLOBALCLASS; // 指示窗口类是应用程序全局类 https://docs.microsoft.com/zh-cn/windows/win32/winmsg/window-class-styles
    //wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH); // 防止背景重绘
    //wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = title;
    wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
    ASSERT(0 != RegisterClassEx(&wcex), "Unable to register a window");

    // --------------------------------------------------------------------------
    // 调整窗口大小
    RECT rc = { 0, 0, (LONG)screenWidth, (LONG)screenHeight };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    // 创建窗口
    g_hwnd = CreateWindowEx(
        0,                      // Optional window styles
        title,                  // Window class
        title,                  // Window text
        WS_OVERLAPPEDWINDOW,    // top-level window style
        CW_USEDEFAULT,          // default horizontal position 
        CW_USEDEFAULT,          // default vertical position 
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,                // Parent window
        nullptr,                // Menu
        hInstance,              // 应用程序实例的句柄
        nullptr                 // Additional application data
    );
    if (g_hwnd == NULL)
    {
        DWORD dwError = GetLastError();
        ASSERT(0);
        return HRESULT_FROM_WIN32(dwError);
    }

    // --------------------------------------------------------------------------
    // InitializeApplication
    //D3D12_VERTEX_BUFFER_VIEW stVertexBufferView={};

    //CD3DX12_VIEWPORT stViewPort(0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight));

    UINT nDXGIFactoryFlags = 0U;

    // --------------------------------------------------------------------------
    // 启用调试层
#ifdef _DEBUG
    // 启用调试层（需要图形工具“可选功能”）。
    // 注意：在设备创建后启用调试层将使活动设备无效。
    winrt::com_ptr<ID3D12Debug3> pID3D12Debug3;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(pID3D12Debug3.put()))))
    {
        pID3D12Debug3->EnableDebugLayer();
        nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif // _DEBUG




    // --------------------------------------------------------------------------
    // 创建 DirectX 图形基础设施 (DXGI) 
    winrt::com_ptr<IDXGIFactory7> pIDXGIFactory7;
    CHECK_HRESULT(CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory7)));
    //GRS_THROW_IF_FAILED(pIDXGIFactory7->MakeWindowAssociation(g_hwnd, DXGI_MWA_NO_ALT_ENTER)); // 防止 DXGI 响应 Alt+Enter 

    // --------------------------------------------------------------------------
    // 创建D3D12设备对象接口
    winrt::com_ptr<IDXGIAdapter4> pIDXGIAdapter4;
    winrt::com_ptr<ID3D12Device6> pID3D12Device6;
    SIZE_T MaxSize = 0;
    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pIDXGIFactory7->EnumAdapters1(adapterIndex, (IDXGIAdapter1**)(pIDXGIAdapter4.put())); adapterIndex++)
    {
        // 遍历所有适配器
        DXGI_ADAPTER_DESC3 desc;
        pIDXGIAdapter4->GetDesc3(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {} // 软件虚拟适配器，跳过
        else if (desc.DedicatedVideoMemory > MaxSize) // 选择最大专用适配器内存（显存）
        {
            auto hr = D3D12CreateDevice(pIDXGIAdapter4.get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(pID3D12Device6.put()));
            if (SUCCEEDED(hr))
            {
                TRACE(L"D3D12-capable hardware found:  %s (%u MB)\n", desc.Description, desc.DedicatedVideoMemory >> 20);
                MaxSize = desc.DedicatedVideoMemory;
            }
        }
        // 重新设定智能指针
        pIDXGIAdapter4 = nullptr;
    }
    if (MaxSize > 0)
        g_Device = std::move(pID3D12Device6);
    ASSERT(g_Device != nullptr, L"D3D12设备对象创建失败");

    // --------------------------------------------------------------------------
    // 创建D3D12命令队列接口
    winrt::com_ptr<ID3D12CommandQueue> pID3D12CommandQueue;
    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    CHECK_HRESULT(g_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pID3D12CommandQueue)));
    pID3D12CommandQueue->SetName(L"Graphics Command Queue");


    // --------------------------------------------------------------------------
    // 创建交换链
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{}; // 交换链描述
    swapChainDesc.Width = ScreenWidth;                              // 分辨率宽度
    swapChainDesc.Height = ScreenHeight;                            // 分辨率高度
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;              // 显示格式
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // 后台缓冲区的表面使用情况和 CPU 访问选项
    swapChainDesc.BufferCount = SWAP_FRAME_BACK_BUFFER_COUNT;       // 交换链中缓冲区数量
    swapChainDesc.SampleDesc.Count = 1;                             // 多采样数量
    swapChainDesc.SampleDesc.Quality = 0;                           // 多采样质量
    swapChainDesc.Scaling = DXGI_SCALING_NONE;                      // 标识调整行为
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;       // 交换链使用的表示模型和用于在呈现表面后处理演示缓冲区内容的选项
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;               // 交换链后台缓冲区的透明度行为
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;   // 指定交换链行为

    //DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc  {};
    //fsSwapChainDesc.Windowed = TRUE;

    winrt::com_ptr<IDXGISwapChain1> pIDXGISwapChain1;
    //winrt::com_ptr<IDXGISwapChain4> pIDXGISwapChain4;
    CHECK_HRESULT(pIDXGIFactory7->CreateSwapChainForHwnd(
        pID3D12CommandQueue.get(),
        g_hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        pIDXGISwapChain1.put()
    ));

    auto pIDXGISwapChain4 = pIDXGISwapChain1.as<IDXGISwapChain4>();
    auto currentBackBufferIndex = pIDXGISwapChain4->GetCurrentBackBufferIndex(); // 当前被绘制的后缓冲序号

    // --------------------------------------------------------------------------
    // 创建RTV描述符堆
    // https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/descriptor-heaps-overview
    // https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/descriptors-overview
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.NumDescriptors = SWAP_FRAME_BACK_BUFFER_COUNT;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    winrt::com_ptr<ID3D12DescriptorHeap> pID3D12DescriptorHeap;
    CHECK_HRESULT(g_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(pID3D12DescriptorHeap.put())));
    auto descriptorHandleIncrementSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); // 查询每个描述符元素的大小

    // 创建RTV描述符  RTV（渲染目标视图）
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pID3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    winrt::com_ptr<ID3D12Resource1> pID3D12Resource1[SWAP_FRAME_BACK_BUFFER_COUNT];
    for (UINT i = 0; i < SWAP_FRAME_BACK_BUFFER_COUNT; i++)
    {
        CHECK_HRESULT(pIDXGISwapChain1->GetBuffer(i, IID_PPV_ARGS(pID3D12Resource1[i].put())));
        g_Device->CreateRenderTargetView(pID3D12Resource1[i].get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, descriptorHandleIncrementSize);
    }

    // --------------------------------------------------------------------------
    // 创建根签名
    // https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/root-signatures-overview
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    winrt::com_ptr<ID3DBlob> pSignature;
    winrt::com_ptr<ID3DBlob> pErrorSignature;

    winrt::com_ptr<ID3D12RootSignature> pID3D12RootSignature;

    CHECK_HRESULT(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.put(), pErrorSignature.put()));
    CHECK_HRESULT(g_Device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(pID3D12RootSignature.put())));


    // --------------------------------------------------------------------------
    // 编译Shader，创建渲染管线状态对象接口
    winrt::com_ptr<ID3DBlob> vertexShader;
    winrt::com_ptr<ID3DBlob> pixelShader;
    winrt::com_ptr<ID3DBlob> errorBlob;

#ifdef _DEBUG
    // 使用图形调试工具启用着色器调试
    auto compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    auto compileFlags = 0;
#endif // _DEBUG

    HRESULT compileResult;
    compileResult = D3DCompileFromFile(
        L"F:\\DxProject\\RenderingLearn\\Assets\\Shaders\\Sample.hlsl",
        nullptr, nullptr, "VSMain", "vs_5_0",
        compileFlags, 0,
        vertexShader.put(), errorBlob.put()
    );
    if (FAILED(compileResult) && errorBlob)
        TRACE((char*)errorBlob->GetBufferPointer());
    errorBlob = nullptr;

    compileResult = D3DCompileFromFile(
        L"F:\\DxProject\\RenderingLearn\\Assets\\Shaders\\Sample.hlsl",
        nullptr, nullptr, "PSMain", "ps_5_0",
        compileFlags, 0,
        pixelShader.put(), errorBlob.put()
    );
    if (FAILED(compileResult) && errorBlob)
        TRACE((char*)errorBlob->GetBufferPointer());
    errorBlob = nullptr;

    // 定义顶点输入层
    //D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    //{
    //    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    //    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    //};
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
    //std::vector<D3D12_INPUT_ELEMENT_DESC> outputElements;
    //outputElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    //outputElements.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

    // 描述和创建图形管道状态对象 (PSO)
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    //psoDesc.InputLayout = { outputElements.data(), (UINT)outputElements.size() };
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = pID3D12RootSignature.get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    winrt::com_ptr<ID3D12PipelineState> pID3D12PipelineState;
    CHECK_HRESULT(g_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(pID3D12PipelineState.put())));


    // --------------------------------------------------------------------------
    // 创建顶点缓冲
    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

    auto m_aspectRatio = ScreenWidth / (float)ScreenHeight;
    Vertex vertices[] =
    {
        { { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };
    const UINT vertexBufferSize = sizeof(vertices);

    winrt::com_ptr<ID3D12Resource> pID3D12Resource;
    // 注意：不推荐使用上传堆来传输静态数据，如垂直缓冲区。 每次 GPU 需要它时，上传堆都会被编组。 请阅读默认堆用法。 此处使用上传堆是为了代码简单，并且因为要实际传输的顶点很少。
    auto pHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto pDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices));
    CHECK_HRESULT(g_Device->CreateCommittedResource(
        &pHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &pDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(pID3D12Resource.put())));

    // 将三角形数据复制到顶点缓冲区
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0); // 不在 CPU 上读取此资源
    CHECK_HRESULT(pID3D12Resource->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, vertices, vertexBufferSize);
    pID3D12Resource->Unmap(0, nullptr);

    // 初始化顶点缓冲区视图
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    vertexBufferView.BufferLocation = pID3D12Resource->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(Vertex); // 指定缓冲区的大小
    vertexBufferView.SizeInBytes = vertexBufferSize;


    // --------------------------------------------------------------------------
    // 创建命令列表分配器
    winrt::com_ptr<ID3D12CommandAllocator> pID3D12CommandAllocator;
    CHECK_HRESULT(g_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(pID3D12CommandAllocator.put())));
    // 创建图形命令列表
    winrt::com_ptr<ID3D12GraphicsCommandList5> pID3D12GraphicsCommandList5;
    CHECK_HRESULT(g_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pID3D12CommandAllocator.get(), pID3D12PipelineState.get(), IID_PPV_ARGS(pID3D12GraphicsCommandList5.put())));
    // 命令列表是在录制状态下创建的，但还没有什么可录制的。 主循环希望它关闭，所以现在关闭它。
    pID3D12GraphicsCommandList5->Close();


    // --------------------------------------------------------------------------
    // 创建同步对象 Fence， 用于等待渲染完成
    winrt::com_ptr<ID3D12Fence1> pID3D12Fence1;
    CHECK_HRESULT(g_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(pID3D12Fence1.put())));
    UINT64 fenceValue = 1;

    // 创建用于帧同步的事件句柄，用于等待Fence事件通知
    auto fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fenceEvent == nullptr)
        CHECK_HRESULT(HRESULT_FROM_WIN32(GetLastError()));

    // 等待命令列表执行。 我们在主循环中重用相同的命令列表，但现在，我们只想等待设置完成后再继续。
    // void D3D12HelloTriangle::WaitForPreviousFrame()
    auto WaitForPreviousFrame = [&]()
    {
        // 在继续之前等待框架完成不是最佳实践。 为简单起见，这是这样实现的代码。 D3D12HelloFrameBuffering 示例说明了如何使用围栏来有效利用资源并最大限度地提高 GPU 利用率。

        // 发出信号并增加围栏值。
        const UINT64 fence = fenceValue;
        CHECK_HRESULT(pID3D12CommandQueue->Signal(pID3D12Fence1.get(), fence));
        fenceValue++;

        // 等到前一帧完成。
        if (pID3D12Fence1->GetCompletedValue() < fence)
        {
            CHECK_HRESULT(pID3D12Fence1->SetEventOnCompletion(fence, fenceEvent));
            WaitForSingleObject(fenceEvent, INFINITE);
        }

        currentBackBufferIndex = pIDXGISwapChain4->GetCurrentBackBufferIndex();
    };
    WaitForPreviousFrame();


    // --------------------------------------------------------------------------
    ShowWindow(g_hwnd, nCmdShow);

    MSG msg;
    while (!ExitFlag)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                ExitFlag = true;
        }

        // --------------------------------------------------------------------------
        // 渲染
        // 填充命令列表
        {
            // 命令列表分配器只能在相关命令列表在 GPU 上完成执行时重置, 应用程序应使用围栏来确定 GPU 执行进度。
            CHECK_HRESULT(pID3D12CommandAllocator->Reset());

            // 但是，当对特定命令列表调用 ExecuteCommandList() 时，该命令列表可以随时重置，并且必须在重新录制之前重置。
            CHECK_HRESULT(pID3D12GraphicsCommandList5->Reset(pID3D12CommandAllocator.get(), pID3D12PipelineState.get()));

            // 设置必要的状态。
            pID3D12GraphicsCommandList5->SetGraphicsRootSignature(pID3D12RootSignature.get());
            auto viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));
            pID3D12GraphicsCommandList5->RSSetViewports(1, &viewport);
            auto scissorRect = CD3DX12_RECT(0, 0, ScreenWidth, ScreenHeight);
            pID3D12GraphicsCommandList5->RSSetScissorRects(1, &scissorRect);

            // 指示后台缓冲区将用作渲染目标。
            currentBackBufferIndex = pIDXGISwapChain4->GetCurrentBackBufferIndex();
            auto barriers1 = CD3DX12_RESOURCE_BARRIER::Transition(pID3D12Resource1[currentBackBufferIndex].get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            pID3D12GraphicsCommandList5->ResourceBarrier(1, &barriers1);

            auto rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(pID3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), currentBackBufferIndex, descriptorHandleIncrementSize);
            // 设置渲染目标
            pID3D12GraphicsCommandList5->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);


            // 记录命令
            const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
            pID3D12GraphicsCommandList5->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
            pID3D12GraphicsCommandList5->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            pID3D12GraphicsCommandList5->IASetVertexBuffers(0, 1, &vertexBufferView);
            pID3D12GraphicsCommandList5->DrawInstanced(3, 1, 0, 0);


            // 指示现在将使用后台缓冲区来呈现。
            auto barriers2 = CD3DX12_RESOURCE_BARRIER::Transition(pID3D12Resource1[currentBackBufferIndex].get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            pID3D12GraphicsCommandList5->ResourceBarrier(1, &barriers2);

            CHECK_HRESULT(pID3D12GraphicsCommandList5->Close());
        }

        // 执行命令列表
        ID3D12CommandList* ppCommandLists[] = { pID3D12GraphicsCommandList5.get() };
        pID3D12CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // 呈现帧。
        CHECK_HRESULT(pIDXGISwapChain4->Present(1, 0));

        WaitForPreviousFrame();
    }

    // 一般向程序消息循环
    //while (GetMessage(&msg, NULL, 0, 0))
    //{
    //    TranslateMessage(&msg);
    //    DispatchMessage(&msg);
    //}

    return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
    auto result = InitializeApplication(hInstance, nCmdShow);

    return 0;
}