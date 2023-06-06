#include "pch.h"
#include "​GraphicsCommand/CommandAllocator.h"


// --------------------------------------------------------------------------
/*
    命令分配器（CommandAllocator）
        命令分配器可让应用管理分配给命令列表的内存，对应于存储 GPU 命令的底层分配。一个给定的分配器可同时与多个“当前正在记录”命令列表相关联，不过，可以使用一个命令分配器来创建任意数量的 GraphicsCommandList 对象。


    设计思路：
        使用对象池来管理命令分配器，
*/
// --------------------------------------------------------------------------

using namespace std;
using namespace winrt;

namespace D3D12Core
{
}