#include "./Phong.hlsli"


cbuffer camera : register(b0)
{
    float3 position;
}

cbuffer light : register(b1)
{
    float3 color;
    float3 direction;
}

Texture2D<float4> model_tex : register(t0);
 
Texture2D<float4> world_position_tex : register(t1);
Texture2D<float4> normal_tex : register(t2);
Texture2D<float4> color_tex : register(t3);
Texture2D<float4> albedo_tex : register(t4);

SamplerState smp : register(s0);

float4 PSMain(VSOutput vsOut) : SV_TARGET
{
    float4 model = model_tex.Sample(smp, vsOut.texcoord);
    float3 normal = normal_tex.Sample(smp, vsOut.texcoord).xyz;
    float4 albedo = albedo_tex.Sample(smp, vsOut.texcoord);
    
    float3 position = world_position_tex.Sample(smp, vsOut.texcoord).xyz;
    
    // ŠgŽU”½ŽËŒõ‚ðŒvŽZ
    float3 lig = 0.0f;
    float t = max(0.0f, dot(normal, direction) * -1.0f);
    lig = color * t;
    
    float3 r = reflect(direction, normal);
    t = max(0.0f, dot(position, r));
    t = pow(t, 5.0f);
    lig += color * t;
    
    float4 finalColor = albedo;
    finalColor.xyz *= lig;
    return finalColor;
}