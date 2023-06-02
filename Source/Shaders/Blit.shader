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
    
    float4 color = SourceTexture.Sample(SourceTextureSampler, uv);
    
    // TODO 颜色映射暂时写在这
    color.rgb = color.rgb / (color.rgb + 1.0);
    color.rgb = pow(color.rgb, 1.0 / 2.2);
    
    return color;
}
