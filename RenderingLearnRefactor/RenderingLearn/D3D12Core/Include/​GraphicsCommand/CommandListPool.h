#pragma once

namespace D3D12Core
{
    class ICommandList;
    class GraphicsCommandList;

    class CommandListPool
    {
    public:
        CommandListPool() = delete;

        template <class T>
        static T* Request()
        {
            static_assert(std::is_base_of<ICommandList, T>::value, TEXT("类型必须是 ICommandList 的子类"));
            if (typeid(T) == typeid(GraphicsCommandList))
            {
                return static_cast<T*>(RequstImpl(D3D12_COMMAND_LIST_TYPE_DIRECT));
            }
            if (typeid(T) == typeid(ComputeCommandList))
            {
                return static_cast<T*>(RequstImpl(D3D12_COMMAND_LIST_TYPE_COMPUTE));
            }
            if (typeid(T) == typeid(CopyCommandList))
            {
                return static_cast<T*>(RequstImpl(D3D12_COMMAND_LIST_TYPE_COPY));
            }
            return nullptr;
        }

        static void Restore(ICommandList** commandList);

    private:
        static ICommandList* RequstImpl(D3D12_COMMAND_LIST_TYPE type);

    };
}
