#include "./MultiPathCompositing.hlsli"

// 頂点シェーダーへの入力頂点構造体
struct VSInput
{
    float4 pos : POSITION; //XYZW
    float2 texcoord : TEXCOORD; //UV
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = input.pos;
    output.texcoord = input.texcoord;
    return output;
}