
#include "./Rotation.hlsli"

//アフィン変換行列
cbuffer ConstantBuffer : register(b0)
{
    float4x4 transformMatrix;
}

// 頂点シェーダーへの入力頂点構造体
struct VSInput
{
    float4 pos : POSITION; //XYZW
    float4 color : COLOR; //RGBA
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    //output.position = input.pos;
    output.position = mul(input.pos, transformMatrix);
    //output.position = mul(transformMatrix, input.pos);
    output.color = input.color;

    return output;
}