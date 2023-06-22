#include "./Triangle.hlsli"

// 頂点シェーダー
// 1. 引数は変換前の頂点情報
// 2. 戻り値は変換後の頂点情報
VSOutput VSmain(VSInput In)
{
    VSOutput vsOut = (VSOutput) 0;
    vsOut.pos = mul(g_worldMatrix, In.pos);
    vsOut.uv = In.uv;
    vsOut.color = In.color; //
    return vsOut;
}