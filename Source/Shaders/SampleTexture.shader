struct VSInput
{
    float4 position : SV_POSITION;
    float4 normal   : NORMAL;
    float4 color    : COLOR;
    float4 uv       : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 worldPos : TEXCOORD2;
    float4 color : COLOR;
    float4 normal   : NORMAL;
    float4 uv : TEXCOORD;
};

// 常量缓冲区
cbuffer MVPBuffer : register(b0)
{
    float4x4 m_P;
    float4x4 m_V;
    float4x4 m_M;
    float4x4 m_IT_M;
    float4x4 m_MVP;
};

Texture2D<float4> g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(VSInput appdata)
{
    PSInput f;

    //f.position = position;
    f.position = mul(appdata.position, m_MVP);
    //f.position = mul(m_MVP, position);
    //f.position = mul(mul(mul(position, m_M), m_V), m_P);
    //f.position = mul(m_P, mul(m_V, mul(m_M, position)));
    //f.worldPos = position;
    f.worldPos = mul(appdata.position, m_M);
    //f.worldPos = mul(m_M, position);
    //f.color = appdata.color;

    f.normal = appdata.normal;
    //f.normal = mul(appdata.normal, m_IT_M);
    f.color = appdata.color;
    f.uv = appdata.uv;

    return f;
}

float4 PSMain(PSInput input) : SV_TARGET
{
     //return float4(input.uv.xy, 0.0f, 1.0f);
     //return float4(input.worldPos.xyz, 1.0f);
     return float4(input.uv.rgb, 1.0f);
     //return float4(input.color.rgb, 1.0f);
     //return float4(input.normal.rgb, 1.0f);
    //return g_texture.Sample(g_sampler, input.uv.xy);
}
