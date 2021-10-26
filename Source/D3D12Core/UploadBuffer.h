#pragma once

#include "GpuResource.h"

class UploadBuffer : public GpuResource
{
public:
    UploadBuffer();

    void Create(UINT64 size);

private:

};
