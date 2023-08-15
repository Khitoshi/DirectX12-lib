#include "./3dModel.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラ

float4 PSMain(VSOutput input) : SV_TARGET
{
    //return float4(tex.Sample(smp, input.texcoord) * input.diffuse_color);
    return float4(tex.Sample(smp, input.texcoord));
    //return float4(1.0f, 0, 0, 1.0f);
}