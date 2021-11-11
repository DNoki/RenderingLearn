#pragma once

#include "GpuResource.h"

class UploadBuffer : public GpuResource
{
public:
    static UploadBuffer* Request(UINT64 size);

    UploadBuffer();

    void Create(UINT64 size);

private:
    UINT64 m_Size;
};
