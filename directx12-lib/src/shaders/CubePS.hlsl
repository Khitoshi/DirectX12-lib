#include "./Cube.hlsli"

float4 PSMain(VSOutput input) : SV_TARGET
{


    return float4(input.texcoord, 0.0f, 1.0f);
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);

}