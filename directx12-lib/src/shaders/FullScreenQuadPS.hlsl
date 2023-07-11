#include "./FullScreenQuad.hlsli"

float4 PSMain(VSOutput input) : SV_TARGET
{
    return float4(input.uv, 1.0f, 1.0f);
}