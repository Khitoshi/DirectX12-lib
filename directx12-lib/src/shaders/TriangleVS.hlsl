#include "./Triangle.hlsli"


    // 頂点シェーダーへの入力頂点構造体
struct VSInput
{
    float4 pos : POSITION; //XYZW
    float4 color : COLOR; //RGBA
};

// 頂点シェーダー
// 1. 引数は変換前の頂点情報
// 2. 戻り値は変換後の頂点情報
VSOutput VSMain(VSInput In)
{
    VSOutput result = (VSOutput) 0;
    result.pos = In.pos;
    result.color = In.color;
    return result;
}