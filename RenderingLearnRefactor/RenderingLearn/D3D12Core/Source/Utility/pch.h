#pragma once

#include "D3D12Core.h"

#include "​GraphicsCommand/CommandAllocator.h"
#include "​GraphicsCommand/CommandAllocatorPool.h"
#include "​GraphicsCommand/ICommandList.h"
#include "​GraphicsResource/DescriptorAllocator.h"
#include "​GraphicsResource/IPipelineState.h"
#include "​GraphicsResource/IGraphicsResource.h"
#include "​GraphicsResource/PlacedHeap.h"
#include "​GraphicsResource/GraphicsMemory.h"


// 边界对齐，B为2的指数倍
#define UINT_UPPER(A, B) (((uint32)A + (uint32)B - 1u)&~((uint32)B - 1u))
#define UINT64_UPPER(A, B) (((uint64)A + (uint64)B - 1u)&~((uint64)B - 1u))


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
