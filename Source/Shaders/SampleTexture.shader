#include "ShaderCommon.inl"

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

Texture2D<float4> g_texture : register(t0);
SamplerState g_sampler : register(s0);

Texture2D<float4> DirLightShadowMap : register(t1);
SamplerComparisonState DirLightShadowMapState : register(s1);


PSInput VSMain(VSInput appdata)
{
    PSInput f;

    f.position = mul(appdata.position, _Model_MVP);
    f.worldPos = mul(appdata.position, _Model_LocalToWorld);

    //f.color = appdata.color;

    //f.normal = appdata.normal;
    f.normal = mul(appdata.normal, _Model_IT_M);
    f.color = appdata.color;
    f.uv = appdata.uv;

    return f;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float2 uv = input.uv.xy;
    float2 screenUV = float2(input.position.x / 1920, input.position.y / 1080);
    float3 lightDir = -normalize(_DirLight_WorldPos.xyz);
    float3 normal = normalize(input.normal.xyz);

    float4 lightSpacePos = mul(input.worldPos, _DirLight_WorldToLight);
    lightSpacePos = mul(lightSpacePos, _DirLight_LightToClip);
    lightSpacePos = float4(lightSpacePos.xyz / lightSpacePos.w, lightSpacePos.w);
    //float3 projCoords = float3(lightSpacePos.xyz * 0.5f + 0.5f);
    float3 projCoords = float3(lightSpacePos.xy * 0.5f + 0.5f, lightSpacePos.z);
    projCoords.y = 1.0f - projCoords.y;

    float diffuse = clamp(dot(lightDir, normal), 0.0f, 1.0f);

    //float depth = DirLightShadowMap.SampleCmp(DirLightShadowMapState, projCoords.xy, projCoords.z + 0.001f);
    //float depth = DirLightShadowMap.Sample(g_sampler, projCoords.xy).r;
    //depth = depth > projCoords.z + 0.001f ? 0.0f : 1.0f;

    float shadow = 1.0f;
    {
        // 4 Samples
        float sum = 0.0f;
        float count = 2;
        [unroll(8)] for (int x = 0; x < count; x++)
            [unroll(8)] for (int y = 0; y < count; y++)
            {
                float2 sampleUV = projCoords.xy + (float2(x, y) - (count * 0.5f - 0.5f)) * float2(1.0f / 2048, 1.0f / 2048) * 1.0f;

                sum += DirLightShadowMap.SampleCmp(DirLightShadowMapState, sampleUV, projCoords.z + 0.003f);
            }
        shadow = sum / (count * count);
    }

    //float3 color = g_texture.Sample(g_sampler, input.uv.xy).rgb;
    float3 color = float3(1.0f, 1.0f, 1.0f);
    color *= diffuse * _DirLight_Color.rgb * shadow;

    //if (screenUV.x < 0.5f)
    //    return float4(DirLightShadowMap.Sample(g_sampler, projCoords.xy).rrr, 1.0f);
    //else return float4(depth.xxx, 1.0f);

#if 0
    // 从顶点着色器输出的裁切空间顶点，经过以下变换后输入像素着色器
    float4 inputPos = input.clipPos;
    // ① xyz 透视除法
    inputPos = float4(inputPos.xyz / inputPos.w, inputPos.w);
    // ② xy 从 [-1, 1] 映射到 [0, 1] （屏幕空间UV）
    inputPos = float4(inputPos.xy * 0.5f + 0.5f, inputPos.zw);
    // ③ DirectX 翻转纵坐标，以屏幕左上角为原点
    inputPos.y = 1.0f - inputPos.y;
    // ④ 映射到屏幕坐标
    inputPos = float4(inputPos.x * 1920, inputPos.y * 1080, inputPos.zw);
#endif

    return float4(color, 1.0f);
    //return float4(input.uv.xy, 0.0f, 1.0f);
    //return float4(input.position.xyz, 1.0f);
    //return float4(input.uv.rgb, 1.0f);
    //return float4(input.color.rgb, 1.0f);
    //return float4(input.normal.rgb, 1.0f);
    //return g_texture.Sample(g_sampler, input.uv.xy);
}
