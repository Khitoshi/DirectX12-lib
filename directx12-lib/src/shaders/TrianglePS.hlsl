#include "./Triangle.hlsli"


// ピクセルシェーダー
float4 PSMain(VSOutput vsOut) : SV_Target0
{
    return vsOut.color;
}