#include "pch.h"


#include "ConstansBuffer.h"


namespace Game
{
    std::map<UINT64, void*> ConstansBufferPool::s_ConstansBufferQueues{};
}