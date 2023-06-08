#pragma once

#include "D3D12Core.h"

// 边界对齐，B为2的指数倍
#define UINT_UPPER(A, B) (((UINT)A + (UINT)B - 1u)&~((UINT)B - 1u))
#define UINT64_UPPER(A, B) (((UINT64)A + (UINT64)B - 1u)&~((UINT64)B - 1u))

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
