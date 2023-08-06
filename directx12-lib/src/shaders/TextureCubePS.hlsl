#include "./TextureCube.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラ

float4 PSMain(VSOutput input) : SV_TARGET
{
    return float4(tex.Sample(smp, input.texcoord));
    //return float4(input.texcoord, 0, 1.0f);

}