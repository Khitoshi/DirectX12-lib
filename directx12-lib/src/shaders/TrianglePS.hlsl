#include "./Triangle.hlsli"

// ピクセルシェーダー
float4 PSmain(VSOutput vsOut) : SV_Target0
{
    // step-5 テクスチャカラーをサンプリングして返す
    return float4(vsOut.color, 1.0f);
}