#include "pch.h"
#include "​GraphicsResource/IPipelineState.h"

// --------------------------------------------------------------------------
/*
    管道状态概述 https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/managing-graphics-pipeline-state-in-direct3d-12

    管道状态对象对应于图形处理单元 (GPU) 状态的重要部分。此状态包括所有当前设置的着色器和某些固定功能状态对象。
    更改管道对象中包含的状态的唯一方法是更改当前绑定的管道对象。

*/
// --------------------------------------------------------------------------

using namespace D3D12Core;
