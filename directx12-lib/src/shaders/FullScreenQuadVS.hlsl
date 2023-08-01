#include "./FullScreenQuad.hlsli"

// 頂点シェーダーへの入力頂点構造体
struct VSInput
{
    float4 pos : POSITION; //XYZW
    float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = input.pos;
    output.uv = input.uv;
    return output;
}