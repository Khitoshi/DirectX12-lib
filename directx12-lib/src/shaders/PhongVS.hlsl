#include "./Phong.hlsli"

struct VSInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput o;
    o.position = float4(input.position, 1.0f);
    o.texcoord = input.texcoord;
    return o;
}