#include "pch.h"
#include "System/WindowApplication.h"

using namespace D3D12Core;
using namespace D3D12Viewer;

WindowApplication WindowApplication::s_Instance = WindowApplication();

void WindowApplication::Run(HINSTANCE hInstance, int32 nCmdShow)
{
    // 初始化窗口
    InitWindow(hInstance);
    // 初始化程序路径
    InitPathes();

    if (g_Hwnd == NULL)
    {
        auto errorHR = HRESULT_FROM_WIN32(GetLastError());
        CHECK_HRESULT(errorHR);
        return;
    }

    // --------------------------------------------------------------------------
    // 初始化图形管理器
    GraphicsManager::GetInstance().Initialize(g_Hwnd);
    //InitializeCommonGraphicsResource();
    //TimeSystem::InitTimeSystem(); // 初始化时间系统
    //Input::Initialize(g_Hwnd); // 初始化输入模块
    //SceneManager::Initialize(0); // 初始化场景管理器

    // --------------------------------------------------------------------------
    // 显示窗口
    ShowWindow(g_Hwnd, nCmdShow);

    Mesh g_Mesh = Mesh::CreateQuad();

    ShaderDesc shaderDesc{};
    shaderDesc.m_SemanticFlags = (1 << (int32)VertexSemantic::Position) | (1 << (int32)VertexSemantic::Normal) | (1 << (int32)VertexSemantic::Texcoord);
    shaderDesc.m_ShaderFilePaths[static_cast<int32>(ShaderType::VertexShader)] = GetShaderPath().append("Sample_vs.cso");
    shaderDesc.m_ShaderFilePaths[static_cast<int32>(ShaderType::PixelShader)] = GetShaderPath().append("Sample_ps.cso");
    shaderDesc.m_CbvCount = 0;
    shaderDesc.m_SrvCount = 1;
    shaderDesc.m_SamplerCount = 1;
    Shader g_Shader{};
    g_Shader.Create(&shaderDesc);
    g_Shader.SetName(TEXT("示例着色器"));


    DescriptorHandle g_SamplerLinearBorder;  // 线性采样边界色纹理
    // 创建动态采样器
    D3D12_SAMPLER_DESC sampler{};
    sampler.MipLODBias = 0.0f;
    sampler.MaxAnisotropy = 1;
    sampler.BorderColor[0] = 0.0f;
    sampler.BorderColor[1] = 0.0f;
    sampler.BorderColor[2] = 0.0f;
    sampler.BorderColor[3] = 0.0f;
    sampler.MinLOD = 0;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;

    g_SamplerLinearBorder = DescriptorAllocator::Allocat(*GraphicsManager::GetInstance().GetGraphicsContext(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    GraphicsManager::GetInstance().GetDevice()->CreateSampler(&sampler, g_SamplerLinearBorder);


    auto g_Material = Material();
    g_Material.Create(&g_Shader);
    g_Material.SetName(TEXT("示例材质"));
    g_Material.SetDepthEnable(false);
    g_Material.BindSampler(0, g_SamplerLinearBorder);

    Path texFilePath = FORMAT(TEXT("%s/云堇.jpg"), g_AssetPath.c_str());
    Vector<uint8> texData; DXGI_FORMAT texFormat;
    int32 texWidth, texHeight;
    D3D12Viewer::LoadTexture2D(texFilePath, texData, texFormat, texWidth, texHeight);

    auto g_Texture = Texture2D();
    g_Texture.Create(texFormat, texWidth, texHeight);
    g_Texture.DispatchCopyTextureData(texData.data());

    g_Material.BindTexture(0, g_Texture);


    GraphicsManager::GetInstance().GetGraphicsCommandQueue()->WaitForQueueCompleted();
    GraphicsManager::GetInstance().GetCopyCommandQueue()->WaitForQueueCompleted();

    MSG msg;
    while (!g_AppEvent.test(EventFlag::Exit))
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                g_AppEvent.set(EventFlag::Exit);
        }

        auto swapChain = GraphicsManager::GetInstance().GetSwapChain();
        auto currentRenderTarget = swapChain->GetRenderTarget(swapChain->GetCurrentBackBufferIndex());

        auto renderPass = RenderPass();
        renderPass.Create();
        renderPass.SetRenderTargets({ currentRenderTarget });
        renderPass.SetRTBeginningAccess(0,
            D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR,
            DXGI_FORMAT_R32G32B32_FLOAT,
            Color(0.0f, 0.5f, 0.75f));
        renderPass.SetRTEndingAccess(0,
            D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE);
        renderPass.BeginRenderPass();

        renderPass.SetViewports(0, 0, ScreenWidth, ScreenHeight);

        {
            renderPass.DrawCall(&g_Mesh, &g_Material);
        }

        renderPass.EndRenderPass();

        GraphicsManager::GetInstance().GetGraphicsCommandQueue()->ExecuteCommandLists({ renderPass.m_CommandList });
        GraphicsManager::GetInstance().GetGraphicsCommandQueue()->WaitForQueueCompleted();

        CHECK_HRESULT(swapChain->GetD3D12SwapChain()->Present(1, 0));
        //CHECK_HRESULT(swapChain->GetD3D12SwapChain()->Present(0, DXGI_PRESENT_ALLOW_TEARING));

        //    TimeSystem::RefreshTimeSystem();
        //    Input::RefreshBeforeUpdate();
        //    SceneManager::GetActiveScene()->ExecuteScene();
        //    SceneManager::CheckLoadNextScene();


        //    // TODO ALT+F4 关闭窗口
        //    if ((Input::KeyState(KeyCode::LeftAlt) && Input::KeyDown(KeyCode::F4)) || Input::KeyDown(KeyCode::Escape))
        //    {
        //        CloseWindow();
        //    }
    }

    // 一般向程序消息循环
    //while (GetMessage(&msg, NULL, 0, 0))
    //{
    //    TranslateMessage(&msg);
    //    DispatchMessage(&msg);
    //}

    //// 销毁场景管理器
    //SceneManager::Destroy();
    // 销毁图形管理器
    GraphicsManager::GetInstance().Destroy();
}

void WindowApplication::InitWindow(HINSTANCE hInstance)
{
    // 第一个 Windows 程序
    // https://docs.microsoft.com/zh-cn/windows/win32/learnwin32/creating-a-window

    // --------------------------------------------------------------------------
    // 注册窗口类 
    // https://docs.microsoft.com/zh-cn/windows/win32/winmsg/using-window-classes
    WNDCLASSEX wcex{};
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_GLOBALCLASS; // 指示窗口类是应用程序全局类 https://docs.microsoft.com/zh-cn/windows/win32/winmsg/window-class-styles
    //wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProcess;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH); // 防止背景重绘
    //wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = WindowTitle.c_str();
    wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

    ATOM registerResult = RegisterClassEx(&wcex);
    ASSERT(0 != registerResult, L"ERROR::Unable to register a window");

    // --------------------------------------------------------------------------
    // 调整窗口大小
    RECT rc = { 0, 0, static_cast<LONG>(ScreenWidth), static_cast<LONG>(ScreenHeight) };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    // 创建窗口
    g_Hwnd = CreateWindowEx(
        0,                      // Optional window styles
        WindowTitle.c_str(),          // Window class
        WindowTitle.c_str(),          // Window text
        WS_OVERLAPPEDWINDOW,    // top-level window style
        CW_USEDEFAULT,          // default horizontal position 
        CW_USEDEFAULT,          // default vertical position 
        rc.right - rc.left,     // 窗口宽度
        rc.bottom - rc.top,     // 窗口高度
        nullptr,                // Parent window
        nullptr,                // Menu
        hInstance,              // 应用程序实例的句柄
        nullptr                 // Additional application data
    );
}

void WindowApplication::InitPathes()
{
    String path;
    path.resize(MAX_PATH + 1);
    if (GetModuleFileName(NULL, path.data(), MAX_PATH) == 0)
    {
        auto errorHR = HRESULT_FROM_WIN32(GetLastError());
        CHECK_HRESULT(errorHR);
        return;
    }

    g_AppFullPath = Path(path);
    g_AppPath = g_AppFullPath.parent_path();
    g_ProjectPath = g_AppFullPath;
    while (g_ProjectPath.filename() != "RenderingLearn")
    {
        if (!g_ProjectPath.has_parent_path())
            ASSERT(0, L"ERROR::未能找到项目路径");
        g_ProjectPath = g_ProjectPath.parent_path();
    }
    g_AssetPath = g_ProjectPath.parent_path().parent_path();
    //g_AssetPath = g_ProjectPath;
    g_AssetPath.append("Assets");
    //g_ShaderPath = g_ProjectPath;
    //g_ShaderPath.append("Source\\Shaders"); 
    g_ShaderPath = g_AppPath;
    g_ShaderPath.append("CompiledShaders");
}

LRESULT WindowApplication::WindowProcess(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        //case WM_ACTIVATE: // 窗口正在激活或停用
        //    Input::MouseProcessMessage(message, wParam, lParam);
        //    break;

        //case WM_ACTIVATEAPP: // 激活或失去激活
        //    Input::KeyboardProcessMessage(message, wParam, lParam);
        //    Input::MouseProcessMessage(message, wParam, lParam);
        //    TimeSystem::ProcessMsg(message, wParam, lParam);
        //    break;

    case WM_ENTERSIZEMOVE: // 窗口进入调整模式（位置，大小）
    {
        GetInstance().g_AppEvent.set(EventFlag::AdjWindow);
        //GraphicsManager::GetSwapChain()->Resize(8, 8); // 使窗口黑屏
        //TimeSystem::ProcessMsg(message, wParam, lParam);
    }
    break;
    case WM_EXITSIZEMOVE: // 窗口退出调整模式（位置，大小）
    {
        GetInstance().g_AppEvent.reset(EventFlag::AdjWindow);
        RECT clientRect;
        GetClientRect(GetInstance().g_Hwnd, &clientRect);
        //    GraphicsManager::GetSwapChain()->Resize(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top); // 调整窗口大小

        //    TimeSystem::ProcessMsg(message, wParam, lParam);
    }
    break;

    //// 按键消息
    //case WM_KEYDOWN:
    //case WM_KEYUP:
    //case WM_SYSKEYDOWN:
    //case WM_SYSKEYUP:
    //    Input::KeyboardProcessMessage(message, wParam, lParam);
    //    break;

    //    // 鼠标消息
    //case WM_INPUT:
    //case WM_MOUSEMOVE:
    //case WM_LBUTTONDOWN:
    //case WM_LBUTTONUP:
    //case WM_RBUTTONDOWN:
    //case WM_RBUTTONUP:
    //case WM_MBUTTONDOWN:
    //case WM_MBUTTONUP:
    //case WM_MOUSEWHEEL:
    //case WM_XBUTTONDOWN:
    //case WM_XBUTTONUP:
    //case WM_MOUSEHOVER:
    //    Input::MouseProcessMessage(message, wParam, lParam);
    //    break;

    // 当单击激活窗口时，忽略该鼠标事件。
    //case WM_MOUSEACTIVATE:
    //    return MA_ACTIVATEANDEAT;


    case WM_SIZE: // 在窗口的大小更改后发送到窗口
    {
        uint32 width = LOWORD(lParam);
        uint32 height = HIWORD(lParam);
        switch (wParam)
        {
        case SIZE_MAXIMIZED:    // 窗口已最大化
        case SIZE_MAXSHOW:      // 当某个其他窗口恢复到以前的大小时，消息会发送到所有弹出窗口
        case SIZE_MAXHIDE:      // 当某个其他窗口最大化时，消息会发送到所有弹出窗口
        case SIZE_RESTORED:     // 窗口已调整大小，但SIZE_MINIMIZED和SIZE_MAXIMIZED值均不适用
            if (!GetInstance().g_AppEvent.test(EventFlag::AdjWindow)) // 正在调整窗口时忽略
            {
                //GraphicsManager::GetSwapChain()->Resize(width, height); // 调整窗口大小
            }
            break;
        case SIZE_MINIMIZED:    // 窗口已最小化
        default: break;
        }
    }
    break;

    case WM_CLOSE: // 窗口将要关闭时
        DestroyWindow(WindowApplication::GetInstance().g_Hwnd);
        break;

    case WM_DESTROY: // 窗口已关闭
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
