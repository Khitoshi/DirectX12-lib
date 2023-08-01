#include "./FullScreenQuad.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラ

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 texColor = tex.Sample(smp, input.uv);
    return texColor;

}