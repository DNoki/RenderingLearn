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
cbuffer CameraBuffer : register(b0)
{
    float4x4 m_Project;
    float4x4 m_View;
};

cbuffer ModelBuffer : register(b1)
{
    float4x4 m_Model;
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
    //f.position = mul(mul(mul(position, m_Model), m_View), m_Project);
    //f.position = mul(m_Project, mul(m_View, mul(m_Model, position)));
    //f.worldPos = position;
    f.worldPos = mul(appdata.position, m_Model);
    //f.worldPos = mul(m_Model, position);
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
    //return float4(input.uv.rgb, 1.0f);
    //return float4(input.color.rgb, 1.0f);
    //return float4(input.normal.rgb, 1.0f);
    return g_texture.Sample(g_sampler, input.uv.xy);
}
