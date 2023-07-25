#include "./MultiPathCompositing.hlsli"

//オフスクリーンテクスチャ
Texture2D OffScreenTexture0 : register(t0);
Texture2D OffScreenTexture1 : register(t1);


SamplerState sam : register(s0);

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 color0 = OffScreenTexture0.Sample(sam, input.texcoord);
    float4 color1 = OffScreenTexture1.Sample(sam, input.texcoord);


    float4 result;
    result.rgb = color0.rgb * color0.a + color1.rgb * (1 - color0.a);
    result.a = color0.a + color1.a * (1 - color0.a);

    return result;
}