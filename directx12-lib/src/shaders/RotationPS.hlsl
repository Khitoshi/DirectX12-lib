#include "./Rotation.hlsli"
float4 PSMain(VSOutput input) : SV_TARGET
{
    return float4(input.color);
}