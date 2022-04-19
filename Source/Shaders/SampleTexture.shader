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

Texture2D<float4> _SampleTexture : register(t0);
SamplerState _SampleSampler : register(s0);

Texture2D<float4> _DirLightShadowMap : register(t1);
SamplerComparisonState _DirLightShadowMapState : register(s1);


float CalcDirectLightShadow(float4 worldPos)
{
    float shadow = 1.0f;
    
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

    // 世界空间坐标转换到光照裁切空间
    float4 lightSpacePos = mul(worldPos, _DirLight_WorldToLightClip);
    // 透视除法
    lightSpacePos = float4(lightSpacePos.xyz / lightSpacePos.w, lightSpacePos.w);
    // 从 [-1, 1] 空间变换到 [0, 1] 空间
    float3 projCoords = float3(lightSpacePos.xy * 0.5f + 0.5f, lightSpacePos.z);
    projCoords.y = 1.0f - projCoords.y;

    // 计算 Blocker Distance
    float pcssFilterSize = 1.0f;
    {
        const float _LightSize = 4.0f;

        float blockerDistance = 0.0f;
        int blockerCount = 0;
        float sumBlockerDistance = 0.0f;
        float searchWidth = _LightSize * projCoords.z;

        const int BLOCKER_FIND_COUNT = 6;
        float offset = BLOCKER_FIND_COUNT * 0.5f - 0.5f;
        [unroll(BLOCKER_FIND_COUNT)] for (int x = 0; x < BLOCKER_FIND_COUNT; x++)
            [unroll(BLOCKER_FIND_COUNT)] for (int y = 0; y < BLOCKER_FIND_COUNT; y++)
            {
                float2 sampleUV = projCoords.xy + (float2(x, y) - offset) * _DirLight_ShadowmapParams.zw * searchWidth;
                float recordedDistance = _DirLightShadowMap.Sample(_SampleSampler, sampleUV).r;

                if (projCoords.z < recordedDistance)
                {
                    sumBlockerDistance += recordedDistance;
                    blockerCount++;
                }
            }

        blockerDistance = sumBlockerDistance / blockerCount;

        if(blockerCount > 0)
            blockerDistance = sumBlockerDistance / blockerCount;
        else return shadow;

        pcssFilterSize = (blockerDistance - projCoords.z) / blockerDistance * _LightSize;
        pcssFilterSize *= pcssFilterSize;
        //int pcssFilterCount = clamp(ceil(pcssFilterSize * 8), 1, 8);
    }
    
    // 4 Samples
    float sum = 0.0f;
    float count = 4;
    float offset = count * 0.5f - 0.5f;
    [unroll(8)] for (int x = 0; x < count; x++)
        [unroll(8)] for (int y = 0; y < count; y++)
        {
        float2 sampleUV = projCoords.xy + ((float2(x, y) - offset) * _DirLight_ShadowmapParams.zw * 8.0f * pcssFilterSize);
            sum += _DirLightShadowMap.SampleCmp(_DirLightShadowMapState, sampleUV, projCoords.z);
        }
    shadow = sum / (count * count);

    return shadow;
}

float3 BlinnPhongRenderModel(float3 normal, float3 lightDir, float3 viewDir)
{
    // 最终光照 = 环境光(Ambient) + 漫反射(Diffuse) + 高光（Specular）

    float3 halfDir = normalize(lightDir + viewDir);

    float diffuse = max(dot(normal, lightDir), 0.0f);

    float specular = pow(max(dot(normal, halfDir), 0.0f), 256.0f);

    return (diffuse + specular).xxx;
}


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
    float2 screenUV = float2(input.position.x * _Camera_ScreenParams.z, input.position.y * _Camera_ScreenParams.w);
    float4 worldPos = input.worldPos;
    float3 normal = normalize(input.normal.xyz);
    float3 lightDir = normalize(-_DirLight_WorldPos.xyz);
    float3 viewDir = normalize(_Camera_WorldPos.xyz - worldPos.xyz);


    float shadow = CalcDirectLightShadow(worldPos);

    //float3 color = _SampleTexture.Sample(_SampleSampler, input.uv.xy).rgb;

    // 最终光照 = 环境光(Ambient) + 漫反射(Diffuse) + 高光（Specular）
    float3 halfDir = normalize(lightDir + viewDir);
     //float diffuse = max(dot(normal, lightDir), 0.0f);
    float diffuse = dot(normal, lightDir) * 0.5f + 0.5f;
    float specular = pow(max(dot(normal, halfDir), 0.0f), 256.0f);

    float3 color = shadow * diffuse + specular;

    color *= _DirLight_Color.rgb;

    //if (screenUV.x < 0.5f)
    //    return float4(_DirLightShadowMap.Sample(_SampleSampler, projCoords.xy).rrr, 1.0f);
    //else return float4(depth.xxx, 1.0f);


    return float4(color, 1.0f);
    //return float4(input.uv.xy, 0.0f, 1.0f);
    //return float4(input.position.xyz, 1.0f);
    //return float4(input.uv.rgb, 1.0f);
    //return float4(input.color.rgb, 1.0f);
    //return float4(input.normal.rgb, 1.0f);
    //return _SampleTexture.Sample(_SampleSampler, input.uv.xy);
}
