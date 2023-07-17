#include "./MultiPathCompositing.hlsli"

Texture2D baseTexture : register(t0);
Texture2D pathTexture : register(t1);
SamplerState sam : register(s0);

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 baseColor = baseTexture.Sample(sam, input.texcoord);
    float4 pathColor = pathTexture.Sample(sam, input.texcoord);

    float4 result = (baseColor + pathColor) / 2;
    //float4 result = pathColor;

    return result;
}