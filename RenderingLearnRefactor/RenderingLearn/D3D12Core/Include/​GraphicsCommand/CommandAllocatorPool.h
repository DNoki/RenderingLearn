#pragma once

namespace D3D12Core
{
    /**
     * @brief 命令分配器对象池
    */
    class CommandAllocatorPool
    {
    public:
        CommandAllocatorPool() = delete;

        static CommandAllocator* Request(D3D12_COMMAND_LIST_TYPE type);
        static void Restore(CommandAllocator** allocator);

    private:
    };
}