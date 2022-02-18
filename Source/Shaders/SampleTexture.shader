struct PSInput
{
    float4 position : SV_POSITION;
    float4 worldPos : TEXCOORD2;
    float4 color : COLOR;
    float4 uv : TEXCOORD;
};

// 常量缓冲区
cbuffer MVPBuffer : register(b0)
{
    float4x4 m_P;
    float4x4 m_V;
    float4x4 m_M;
    float4x4 m_MVP;
};

Texture2D<float4> g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float4 uv : TEXCOORD)
{
    PSInput result;

    //result.position = position;
    result.position = mul(position, m_MVP);
    //result.position = mul(m_MVP, position);
    //result.position = mul(mul(mul(position, m_M), m_V), m_P);
    //result.position = mul(m_P, mul(m_V, mul(m_M, position)));
    //result.worldPos = position;
    result.worldPos = mul(position, m_M);
    //result.worldPos = mul(m_M, position);
    result.color = float(0.0f).xxxx;
    result.uv = uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
     //return float4(input.uv.xy, 0.0f, 1.0f);
     return float4(input.worldPos.xyz, 1.0f);
    //return g_texture.Sample(g_sampler, input.uv.xy);
}
