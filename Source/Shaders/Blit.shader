struct VSInput
{
    float4 position : SV_POSITION;
    float4 uv       : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 uv : TEXCOORD;
};


Texture2D<float4> SourceTexture : register(t0);
SamplerState SourceTextureSampler : register(s0);

PSInput VSMain(VSInput v)
{
    PSInput f;

    f.position = v.position;
    f.uv = v.uv;

    return f;
}

float4 PSMain(PSInput f) : SV_TARGET
{
    float2 uv = f.uv.xy;
    uv.y = 1.0f - uv.y;
    return SourceTexture.Sample(SourceTextureSampler, uv);
}
