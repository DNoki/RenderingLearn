#pragma once

#include "IResource.h"
#include "DescriptorHandle.h"
#if 0
/**
 * @brief GPU 资源
*/
class GraphicsResource : public IResource
{
public:
    /**
     * @brief 获取描述符句柄
     * @return 
    */
    inline const DescriptorHandle* GetDescriptorHandle() const { return &m_DescriptorHandle; }

    void Finalize(const DescriptorHandle* descriptorHandle);

protected:
    // 描述符句柄
    // 由描述符引用的资源使用
    DescriptorHandle m_DescriptorHandle;


    GraphicsResource() :m_DescriptorHandle() {}
};

#endif