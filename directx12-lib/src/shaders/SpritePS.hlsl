#include "./Sprite.hlsli"

float4 PSMain(VSOutput output) : SV_TARGET
{
    return float4(output.texcoord, 1.0f, 1.0f);
}