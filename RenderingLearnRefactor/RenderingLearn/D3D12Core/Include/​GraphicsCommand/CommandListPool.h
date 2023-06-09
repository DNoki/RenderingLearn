#pragma once

namespace D3D12Core
{
    class CommandListPool
    {
    public:
        CommandListPool() = delete;

        template <class T>
        static T* Request()
        {
            static_assert(std::is_base_of<ICommandList, T>::value, TEXT("类型必须是 ICommandList 的子类"));
            D3D12_COMMAND_LIST_TYPE type;
            if (typeid(T) == typeid(GraphicsCommandList))
                type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            else if (typeid(T) == typeid(ComputeCommandList))
                type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            else if (typeid(T) == typeid(CopyCommandList))
                type = D3D12_COMMAND_LIST_TYPE_COPY;
            else throw;
            return static_cast<T*>(RequestImpl(type));
        }

        static void Restore(ICommandList** commandList);

    private:
        static ICommandList* RequestImpl(D3D12_COMMAND_LIST_TYPE type);

    };
}
