#include "pch.h"



#include "CommandAllocatorPool.h"


class CommandAllocatorPoolImpl
{
public:
	CommandAllocatorPoolImpl()
	{

	}

	ID3D12CommandAllocator* RequestAllocator(D3D12_COMMAND_LIST_TYPE Type)
	{
		if (m_GraphicsCommandAllocators.empty())
		{

		}
	}

private:
	std::queue<ID3D12CommandAllocator> m_GraphicsCommandAllocators;
};