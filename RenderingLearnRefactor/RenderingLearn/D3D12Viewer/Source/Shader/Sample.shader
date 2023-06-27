struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 uv : TEXCOORD;
};


Texture2D<float4> SourceTexture : register(t0);
SamplerState SourceTextureSampler : register(s0);


PSInput VSMain(
float4 position : SV_POSITION,
float4 color : COLOR,
float4 uv : TEXCOORD
)
{
    PSInput result;

    result.position = position;
    result.color = color;
    result.uv = uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 color = SourceTexture.Sample(SourceTextureSampler, input.uv.xy);
    return color;
    //return input.uv;
    //return float4(1.0, 0.0, 0.0, 1.0);
    //return input.color;
}