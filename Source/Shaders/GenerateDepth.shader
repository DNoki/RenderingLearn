#include "ShaderCommon.inl"

struct VSInput
{
    float4 position : SV_POSITION;
};

struct PSInput
{
    float4 position : SV_POSITION;
};

PSInput VSMain(VSInput v)
{
    PSInput f;

    f.position = mul(v.position, _Model_MVP);

    return f;
}

void PSMain(PSInput f)
{
    
}
